#include <OathSession.h>

#include <Apdu.h>
#include <ByteArray.h>
#include <CcidConnection.h>
#include <Message.h>
#include <Util.h>

namespace authpp {

using bytes = unsigned char[];

OathSession::OathSession(const CcidConnection& connection)
    : connection(connection)
{
    // appid oath = 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01
    auto oathId = ByteArray(bytes { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 });
    Apdu selectOath(0x00, 0xa4, 0x04, 0x00, oathId);

    Log.d("APDU for select: {}", util::byteDataToString(selectOath.getApduData().get(), selectOath.getApduDataSize()));

    Message selectMessage((std::byte)0x6f, selectOath.getApduData());
    int received = 0;

    Log.d("Building oath select {}", selectMessage.toString());

    auto const response = connection.transcieve(std::forward<Message>(selectMessage), &received);
    Log.d("OATH select response: {}", util::byteDataToString(response.get(), received));
    auto responseBytes = response.get();
    Log.d("Response bytes: {:02x} {:02x}",
        responseBytes[received - 2],
        responseBytes[received - 1]);
}

} // namespace authpp
