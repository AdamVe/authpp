#include "Message.h"

#include "ByteArray.h"
#include "Util.h"

namespace authpp {

#define HEADER_SIZE 10

Message::Message(std::byte type, ByteArray&& data)
    : data_size(data.getSize())
    , message_size(data_size + HEADER_SIZE)
    , message_buffer(new std::byte[message_size] {
          type,
          (std::byte)((data_size >> 0) & 0xFF),
          (std::byte)((data_size >> 8) & 0xFF),
          (std::byte)((data_size >> 16) & 0xFF),
          (std::byte)((data_size >> 24) & 0xFF),
          (std::byte)0x00, (std::byte)0x00,
          (std::byte)0x00, (std::byte)0x00,
          (std::byte)0x00 })
{
    std::memcpy((void*)message_buffer + HEADER_SIZE, data.get(), data_size);
}

Message::~Message()
{
    delete[] message_buffer;
}

std::string Message::toString() const
{
    return util::byteDataToString(message_buffer, message_size);
}

std::byte* Message::get() const
{
    return message_buffer;
}

std::size_t Message::size() const
{
    return message_size;
}

} // namespace authpp
