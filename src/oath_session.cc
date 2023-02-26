#include "oath_session.h"

#include <cstddef>
#include <map>
#include <sstream>

#include "apdu.h"
#include "ccid_connection.h"
#include "fmt/fmt_byte_buffer.h"
#include "fmt/fmt_oath.h"
#include "logger.h"
#include "message.h"
#include "util.h"

namespace authpp {

#define APDU_SUCCESS 0x9000
#define APDU_MORE_DATA 0x6100

namespace {

    Logger log("OathSession");

}

const ByteBuffer kOathId { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 };

using sw_t = std::uint16_t;

sw_t GetSw(const ByteBuffer& buffer)
{

    if (buffer.size() < 2) {
        log.e("Invalid data: size < 2");
        return 0x0000;
    }

    std::size_t last_index = buffer.size() - 1;
    return buffer.getByte(last_index - 1) << 8 | buffer.getByte(last_index);
}

bool isSuccess(uint16_t sw) {
    return (sw & APDU_SUCCESS) == APDU_SUCCESS;
}

bool isMoreData(uint16_t sw) {
    return (sw & APDU_MORE_DATA) == APDU_MORE_DATA;
}

ByteBuffer SendInstruction(const CcidConnection& connection, const Apdu& instruction);

ByteBuffer SendInstruction(const CcidConnection& connection, const Apdu& instruction)
{
    Message ccid_message((uint8_t)0x6f, instruction.get());
    auto response = connection.Transcieve(ccid_message);

    auto sw { GetSw(response) };
    if (isSuccess(sw)) {
        log.v("SW is success");
        return response;
    } else if (isMoreData(sw)) {
        response.setSize(response.size()-2);
        while (isMoreData(sw)) {
            Message ccid_message(0x6f, {0x00, 0xa5, 0x00, 0x00});
            auto remaining = connection.Transcieve(ccid_message);
            sw = GetSw(remaining);
            remaining.setSize(remaining.size() - 2);
            auto currentSize = response.size();
            response.setSize(response.size() + remaining.size());
            response.pointTo(currentSize);
            response.putBytes(remaining);
        }
        return response;
    }

    return {};
}

int Parse(const ByteBuffer& buffer, OathSession::MessageData& messageData)
{
    int32_t i = 0;
    while (i < static_cast<int32_t>(buffer.size()) - 2) {
        auto tag = buffer.getByte(i);
        auto length = buffer.getByte(i + 1);
        ByteBuffer data = buffer.getBytes(i + 2, length);
        messageData.emplace_back(OathSession::DataPair { tag, data });

        //log.d("Parsed tag {:02x} with data {}", tag, messageData.back().buffer);

        i += length + 2;
    }
    return messageData.size();
}

ByteBuffer GetData(const OathSession::MessageData& message_data, std::size_t index)
{
    if (index < message_data.size()) {
        return message_data[index].buffer;
    }
    return {};
}

OathSession::MessageData Select(const CcidConnection& connection,
    const ByteBuffer& app_id)
{
    Apdu select_oath(0x00, 0xa4, 0x04, 0x00, app_id);
    auto select_response = SendInstruction(connection, select_oath);

    OathSession::MessageData tags;
    if (Parse(select_response, tags) == -1) {
        log.e("Invalid data: parse failed");
    }

    return tags;
}

OathSession::Version ParseVersion(const OathSession::MessageData& message_data)
{

    ByteBuffer buffer = GetData(message_data, 0);
    if (buffer.size() > 2) {
        return OathSession::Version(buffer.getByte(0), buffer.getByte(1), buffer.getByte(2));
    }
    return OathSession::Version(0, 0, 0);
}

std::string ParseName(const OathSession::MessageData& message_data)
{
    ByteBuffer buffer = GetData(message_data, 1);
    return "TODO";
}

Algorithm ParseAlgorithm(const OathSession::MessageData& message_data)
{
    ByteBuffer buffer = GetData(message_data, 3);

    if (buffer.size() == 0) {
        return { Algorithm::HMAC_SHA1 };
    }
    auto type = buffer.getByte(0);

    if (type == 0x02) {
        return Algorithm::HMAC_SHA256;
    } else if (type == 0x03) {
        return Algorithm::HMAC_SHA512;
    }

    return Algorithm::HMAC_SHA1;
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

        for (auto&& response_buffer : parsed_response) {
            auto credential = Credential::fromByteBuffer(response_buffer.buffer);
            log.d("Found {}", credential);
        }
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
