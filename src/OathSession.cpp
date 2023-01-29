#include "OathSession.h"

#include "Apdu.h"
#include "ByteArray.h"
#include "CcidConnection.h"
#include "Formatters.h"
#include "Message.h"
#include "Util.h"

namespace authpp {

using bytes = unsigned char[];
ByteArray oathId { bytes { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 } };

using sw_t = std::uint16_t;

sw_t getSw(const ByteArray& byteArray)
{
    auto dataSize = byteArray.getDataSize();
    auto data = byteArray.get();
    std::byte b1 = data[dataSize - 2];
    std::byte b2 = data[dataSize - 1];
    Log.d("Response bytes: {:02x} {:02x}", b1, b2);

    return std::to_integer<std::uint16_t>(b1) << 8 | std::to_integer<std::uint8_t>(b2);
}

// void select(const CcidConnection& connection, const ByteArray& appId)
//{
//
//     Apdu selectOath(0x00, 0xa4, 0x04, 0x00, appId);
//
//     Log.d("APDU for select: {}", selectOath.getApduData());
//
//     Message selectMessage((std::byte)0x6f, selectOath.getApduData());
//     int received = 0;
//
//     Log.d("Building oath select {}", selectMessage);
//
//     auto const response = connection.transcieve(std::forward<Message>(selectMessage), &received);
//     Log.d("OATH select response: {}", response);
//     auto sw { getSw(response) };
//     Log.d("Response: {:04x}", sw);
// }

void send_instruction(const CcidConnection& connection, const Apdu& instruction)
{

    Message ccidMessage((std::byte)0x6f, instruction.getApduData());
    int received = 0;

    Log.d("Building message: {}", ccidMessage);

    auto const response = connection.transcieve(std::forward<Message>(ccidMessage), &received);
    Log.d("Response: {}", response);
    auto sw { getSw(response) };
    Log.d("Response SW: {:04x}", sw);
}

void select(const CcidConnection& connection, const ByteArray& appId)
{
    Apdu selectOath(0x00, 0xa4, 0x04, 0x00, appId);
    send_instruction(connection, selectOath);
}

OathSession::OathSession(const CcidConnection& connection)
    : connection(connection)
{
    select(connection, oathId);
}

void OathSession::list_credentials() const
{
    Apdu listApdu(0x00, 0xa1, 0x00, 0x00);
    send_instruction(connection, listApdu);
}

} // namespace authpp
