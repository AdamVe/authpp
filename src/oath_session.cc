#include "oath_session.h"

#include <cstddef>
#include <map>
#include <sstream>

#include "apdu.h"
#include "byte_array.h"
#include "ccid_connection.h"
#include "fmt/fmt_byte_array.h"
#include "fmt/fmt_oath_version.h"
#include "logger.h"
#include "message.h"
#include "util.h"

namespace authpp {

#define APDU_SUCCESS 0x9000
#define TAG_VERSION (std::byte)0x79
#define TAG_NAME (std::byte)0x71
#define TAG_CHALLENGE (std::byte)0x74
#define TAG_ALGORITHM (std::byte)0x7b

namespace {

    Logger log("OathSession");

}

using bytes = unsigned char[];
const ByteArray kOathId { bytes { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 } };

using sw_t = std::uint16_t;

sw_t GetSw(const ByteArray& byte_array)
{
    auto data_size = byte_array.GetDataSize();
    auto data = byte_array.Get();
    return std::to_integer<std::uint16_t>(data[data_size - 2]) << 8
        | std::to_integer<std::uint8_t>(data[data_size - 1]);
}

ByteArray SendInstruction(const CcidConnection& connection,
    const Apdu& instruction)
{
    Message ccid_message((std::byte)0x6f, instruction.getApduData());
    auto const response = connection.Transcieve(std::forward<Message>(ccid_message));
    return response;
}

int Parse(const ByteArray& byte_array, OathSession::MessageData& messageData)
{
    std::size_t i = 0;

    if (byte_array.GetDataSize() < 2 || GetSw(byte_array) != APDU_SUCCESS) {
        log.e("Invalid data");
        return -1;
    }

    while (i < byte_array.GetDataSize() - 2) {
        std::byte tag = byte_array.Get()[i];
        auto length = (std::uint8_t)byte_array.Get(i + 1);
        messageData[tag] = ByteArray(byte_array, i + 2, length);

        log.d("Parsed tag {:02x} with data {}", (int8_t)tag, messageData[tag]);

        i += length + 2;
    }

    return messageData.size();
}

OathSession::MessageData Select(const CcidConnection& connection,
    const ByteArray& app_id)
{
    Apdu select_oath(0x00, 0xa4, 0x04, 0x00, app_id);
    auto select_response = SendInstruction(connection, select_oath);

    OathSession::MessageData tags;
    if (Parse(select_response, tags) != 4) {
        log.e("Invalid response from select");
        return tags;
    };

    log.i("Version: {}", tags[TAG_VERSION]);

    log.d("Parsing select response: {}", select_response);

    return tags;
}

OathSession::Version ParseVersion(const ByteArray& byte_array)
{
    return OathSession::Version(std::to_integer<uint8_t>(byte_array.Get(0)),
        std::to_integer<uint8_t>(byte_array.Get(1)),
        std::to_integer<uint8_t>(byte_array.Get(2)));
}

std::string ParseName(const ByteArray& byte_array)
{
    std::stringstream ss;
    for (std::size_t i = 0; i < byte_array.GetDataSize(); ++i) {
        ss << std::to_integer<char>(byte_array.Get(i));
    }
    return ss.str();
}

OathSession::Algorithm ParseAlgorithm(const ByteArray& byte_array)
{
    std::byte first = byte_array.Get(0);
    if (first == (std::byte)0x02) {
        return OathSession::Algorithm::HMAC_SHA256;
    } else if (first == (std::byte)0x03) {
        return OathSession::Algorithm::HMAC_SHA512;
    }

    return OathSession::Algorithm::HMAC_SHA1;
}

OathSession::OathSession(const CcidConnection& connection)
    : connection(connection)
    , message_data(Select(connection, kOathId))
    , version(ParseVersion(message_data[TAG_VERSION]))
    , name(ParseName(message_data[TAG_NAME]))
    , challenge(message_data[TAG_CHALLENGE])
    , algorithm(ParseAlgorithm(message_data[TAG_ALGORITHM]))
{
    log.d("Opened OathSession version {} / algo {} / challenge {}", version,
        std::to_underlying(algorithm), challenge);
}

void OathSession::ListCredentials() const
{
    Apdu list_apdu(0x00, 0xa1, 0x00, 0x00);
    auto list_response = SendInstruction(connection, list_apdu);
    if (GetSw(list_response) != APDU_SUCCESS) {
        log.e("Failed to get list response");
    }
}

void OathSession::CalculateAll() const
{
    Apdu apdu(0x00, 0xa4, 0x00, 0x00);
    auto calculate_all_response = SendInstruction(connection, apdu);
    if (GetSw(calculate_all_response) != APDU_SUCCESS) {
        log.e("Failed to get calculate_all response");
    }
}

const OathSession::Version& OathSession::GetVersion() const { return version; }

} // namespace authpp
