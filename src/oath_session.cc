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
const ByteArray kOathId{bytes{0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01}};

using sw_t = std::uint16_t;

sw_t GetSw(const ByteArray& byte_array) {
  auto data_size = byte_array.GetDataSize();
  auto data = byte_array.Get();
  return std::to_integer<std::uint16_t>(data[data_size - 2]) << 8 |
         std::to_integer<std::uint8_t>(data[data_size - 1]);
}

ByteArray SendInstruction(const CcidConnection& connection,
                          const Apdu& instruction) {
  Message ccid_message((std::byte)0x6f, instruction.getApduData());
  auto const response =
      connection.Transcieve(std::forward<Message>(ccid_message));
  return response;
}

void Select(const CcidConnection& connection, const ByteArray& app_id) {
  Apdu select_oath(0x00, 0xa4, 0x04, 0x00, app_id);
  auto select_response = SendInstruction(connection, select_oath);
  if (GetSw(select_response) != APDU_SUCCESS) {
    log.e("Failure executing select");
    return;
  }

  log.d("Parsing select response: {}", select_response);
}

OathSession::OathSession(const CcidConnection& connection)
    : connection(connection) {
  Select(connection, kOathId);
}

void OathSession::ListCredentials() const {
  Apdu list_apdu(0x00, 0xa1, 0x00, 0x00);
  auto list_response = SendInstruction(connection, list_apdu);
  if (GetSw(list_response) != APDU_SUCCESS) {
    log.e("Failed to get list response");
  }
}

void OathSession::CalculateAll() const {
  Apdu apdu(0x00, 0xa4, 0x00, 0x00);
  auto calculate_all_response = SendInstruction(connection, apdu);
  if (GetSw(calculate_all_response) != APDU_SUCCESS) {
    log.e("Failed to get calculate_all response");
  }
}

}  // namespace authpp
