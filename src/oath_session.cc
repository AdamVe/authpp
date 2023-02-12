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
        messageData.emplace_back(OathSession::DataPair { tag, ByteArray(byte_array, i + 2, length) });
        // messageData[tag] = ByteArray(byte_array, i + 2, length);

        log.d("Parsed tag {:02x} with data {}", (int8_t)tag, messageData.back().byte_array);

        i += length + 2;
    }

    return messageData.size();
}

ByteArray GetData(const OathSession::MessageData& message_data, std::size_t index)
{
    if (index < message_data.size()) {
        return message_data[index].byte_array;
    }

    return {};
}

OathSession::MessageData Select(const CcidConnection& connection,
    const ByteArray& app_id)
{
    Apdu select_oath(0x00, 0xa4, 0x04, 0x00, app_id);
    auto select_response = SendInstruction(connection, select_oath);

    OathSession::MessageData tags;
    if (Parse(select_response, tags) == -1) {
        log.e("Invalid data");
    }
    return tags;
}

OathSession::Version ParseVersion(const OathSession::MessageData& message_data)
{
    ByteArray byte_array = GetData(message_data, 0);

    if (byte_array.GetDataSize() > 2) {
        return OathSession::Version(std::to_integer<uint8_t>(byte_array.Get(0)),
            std::to_integer<uint8_t>(byte_array.Get(1)),
            std::to_integer<uint8_t>(byte_array.Get(2)));
    }
    return OathSession::Version(0, 0, 0);
}

std::string ParseName(const OathSession::MessageData& message_data)
{
    ByteArray byte_array = GetData(message_data, 1);
    std::stringstream ss;
    for (std::size_t i = 0; i < byte_array.GetDataSize(); ++i) {
        ss << std::to_integer<char>(byte_array.Get(i));
    }
    return ss.str();
}

OathSession::Algorithm ParseAlgorithm(const OathSession::MessageData& message_data)
{
    ByteArray byte_array = GetData(message_data, 3);

    if (byte_array.GetDataSize() == 0) {
        return {};
    }
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
    , version(ParseVersion(message_data))
    , name(ParseName(message_data))
    , challenge(GetData(message_data, 2))
    , algorithm(ParseAlgorithm(message_data))
{
    log.d("Opened OathSession version {} / algo {} / challenge {}", version,
        std::to_underlying(algorithm), challenge);
}

void OathSession::ListCredentials() const
{
    Apdu list_apdu(0x00, 0xa1, 0x00, 0x00);
    auto list_response = SendInstruction(connection, list_apdu);
    MessageData parsed_response;
    if (auto tags_found = Parse(list_response, parsed_response); tags_found > 0) {
        log.d("Success parsing data. {} tags were found", tags_found);
    }

    for (auto&& credential : parsed_response) {

        char c_name[255] { '\0' };
        std::memcpy(c_name, credential.byte_array.Get() + 1, credential.byte_array.GetDataSize() - 1);
        std::string name(c_name);
        log.d("Found account with name `{}`, type X and algorithm Y", name);
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
