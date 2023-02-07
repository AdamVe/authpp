#include "oath_session.h"

#include "apdu.h"
#include "byte_array.h"
#include "ccid_connection.h"
#include "formatters.h"
#include "message.h"
#include "util.h"

namespace authpp {

#define APDU_SUCCESS 0x9000

namespace {

    Logger log("OathSession");

}

using bytes = unsigned char[];
ByteArray oathId { bytes { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 } };

using sw_t = std::uint16_t;

sw_t getSw(const ByteArray& byteArray)
{
    auto dataSize = byteArray.getDataSize();
    auto data = byteArray.get();
    return std::to_integer<std::uint16_t>(data[dataSize - 2]) << 8 | std::to_integer<std::uint8_t>(data[dataSize - 1]);
}

ByteArray send_instruction(const CcidConnection& connection, const Apdu& instruction)
{
    Message ccidMessage((std::byte)0x6f, instruction.getApduData());
    auto const response = connection.transcieve(std::forward<Message>(ccidMessage));
    return response;
}

void select(const CcidConnection& connection, const ByteArray& appId)
{
    Apdu selectOath(0x00, 0xa4, 0x04, 0x00, appId);
    auto select_response = send_instruction(connection, selectOath);
    if (getSw(select_response) != APDU_SUCCESS) {
        log.e("Failure executing select");
        return;
    }

    log.d("Parsing select response: {}", select_response);
}

OathSession::OathSession(const CcidConnection& connection)
    : connection(connection)
{
    select(connection, oathId);
}

void OathSession::list_credentials() const
{
    Apdu listApdu(0x00, 0xa1, 0x00, 0x00);
    auto list_response = send_instruction(connection, listApdu);
    if (getSw(list_response) != APDU_SUCCESS) {
        log.e("Failed to get list response");
    }
}

void OathSession::calculate_all() const
{
    Apdu apdu(0x00, 0xa4, 0x00, 0x00);
    auto calculate_all_response = send_instruction(connection, apdu);
    if (getSw(calculate_all_response) != APDU_SUCCESS) {
        log.e("Failed to get calculate_all response");
    }
}

} // namespace authpp
