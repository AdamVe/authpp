#include "Message.h"

#include "Util.h"

namespace authpp {

#define HEADER_SIZE 10

Message::Message(byte type, byte* data, std::size_t dataSize)
    : messageSize(dataSize + HEADER_SIZE)
    , messageData(new byte[messageSize])
{
    messageData[0] = type;
    messageData[1] = (dataSize >> 24) & 0xFF;
    messageData[2] = (dataSize >> 16) & 0xFF;
    messageData[3] = (dataSize >> 8) & 0xFF;
    messageData[4] = (dataSize >> 0) & 0xFF;
    messageData[5] = 0x00;
    messageData[6] = 0x00;
    messageData[7] = 0x00;
    messageData[8] = 0x00;
    messageData[9] = 0x00;
}

Message::~Message()
{
    delete[] messageData;
}

std::string Message::toString() const
{
    return util::byteDataToString(messageData, messageSize);
}

byte* Message::get() const
{
    return messageData;
}

std::size_t Message::size() const
{
    return messageSize;
}

} // namespace authpp
