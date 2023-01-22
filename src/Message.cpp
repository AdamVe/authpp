#include "Message.h"

#include "Util.h"

namespace authpp {

#define HEADER_SIZE 10

Message::Message(std::byte type, std::byte* data, std::size_t dataSize)
    : messageSize(dataSize + HEADER_SIZE)
    , messageData(new std::byte[messageSize] {
          type,
          (std::byte)((dataSize >> 24) & 0xFF),
          (std::byte)((dataSize >> 16) & 0xFF),
          (std::byte)((dataSize >> 8) & 0xFF),
          (std::byte)((dataSize >> 0) & 0xFF),
          (std::byte)0x00, (std::byte)0x00,
          (std::byte)0x00, (std::byte)0x00,
          (std::byte)0x00 })
{
}

Message::~Message()
{
    delete[] messageData;
}

std::string Message::toString() const
{
    return util::byteDataToString(messageData, messageSize);
}

std::byte* Message::get() const
{
    return messageData;
}

std::size_t Message::size() const
{
    return messageSize;
}

} // namespace authpp
