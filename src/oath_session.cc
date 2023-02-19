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
const Bytes kOathId { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 };

using sw_t = std::uint16_t;

sw_t GetSw(const Bytes& bytes)
{
    uint16_t retval;
    bytes.getI16(retval, bytes.size() - 2);
    return retval;
}

Bytes SendInstruction(const CcidConnection& connection, const Apdu& instruction)
{
    Message ccid_message((uint8_t)0x6f, instruction.get());
    auto const response = connection.Transcieve(std::forward<Message>(ccid_message));
    return response;
}

int Parse(const Bytes& bytes, OathSession::MessageData& messageData)
{
    std::size_t i = 0;

    if (bytes.size() < 2 || GetSw(bytes) != APDU_SUCCESS) {
        log.e("Invalid data");
        return -1;
    }

    while (i < bytes.size() - 2) {
        uint8_t tag;
        bytes.getChar(tag, i);
        uint8_t length;
        bytes.getChar(length, i + 1);
        Bytes data(length);
        bytes.getBytes(data, i + 2);
        messageData.emplace_back(OathSession::DataPair { tag, data });
        // messageData[tag] = ByteArray(byte_array, i + 2, length);

        log.d("Parsed tag {:02x} with data {}", tag, messageData.back().bytes);

        i += length + 2;
    }

    return messageData.size();
}

Bytes GetData(const OathSession::MessageData& message_data, std::size_t index)
{
    if (index < message_data.size()) {
        return message_data[index].bytes;
    }

    return {};
}

OathSession::MessageData Select(const CcidConnection& connection,
    const Bytes& app_id)
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
    Bytes bytes = GetData(message_data, 0);

    if (bytes.size() > 2) {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
        bytes.getChar(major);
        bytes.getChar(minor);
        bytes.getChar(patch);

        return OathSession::Version(major, minor, patch);
    }
    return OathSession::Version(0, 0, 0);
}

std::string ParseName(const OathSession::MessageData& message_data)
{
    Bytes bytes = GetData(message_data, 1);
    return "TODO";
}

OathSession::Algorithm ParseAlgorithm(const OathSession::MessageData& message_data)
{
    Bytes bytes = GetData(message_data, 3);

    if (bytes.size() == 0) {
        return {};
    }
    uint8_t type;
    bytes.getChar(type);

    if (type == (uint8_t)0x02) {
        return OathSession::Algorithm::HMAC_SHA256;
    } else if (type == (uint8_t)0x03) {
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
    Apdu list_apdu((uint8_t)0x00, (uint8_t)0xa1, (uint8_t)0x00, (uint8_t)0x00, Bytes(0));
    auto list_response = SendInstruction(connection, list_apdu);
    MessageData parsed_response;
    if (auto tags_found = Parse(list_response, parsed_response); tags_found > 0) {
        log.d("Success parsing data. {} tags were found", tags_found);
    }

    for (auto&& credential : parsed_response) {

        char c_name[255] { '\0' };
        std::memcpy(c_name, credential.bytes.get_raw() + 1, credential.bytes.size() - 1);
        std::string name(c_name);
        log.d("Found account with name `{}`, type X and algorithm Y", name);
    }
}

void OathSession::CalculateAll() const
{
    Apdu apdu((uint8_t)0x00, (uint8_t)0xa4, (uint8_t)0x00, (uint8_t)0x00, Bytes(0));
    auto calculate_all_response = SendInstruction(connection, apdu);
    if (GetSw(calculate_all_response) != APDU_SUCCESS) {
        log.e("Failed to get calculate_all response");
    }
}

const OathSession::Version& OathSession::GetVersion() const { return version; }

} // namespace authpp
