#include "message.h"

#include "byte_array.h"
#include "util.h"

namespace authpp {

#define HEADER_SIZE 10

Message::Message(std::byte type, ByteArray&& data)
    : data_size(data.GetDataSize()),
      message_size(data_size + HEADER_SIZE),
      message_buffer(new std::byte[message_size]{
          type, (std::byte)((data_size >> 0) & 0xFF),
          (std::byte)((data_size >> 8) & 0xFF),
          (std::byte)((data_size >> 16) & 0xFF),
          (std::byte)((data_size >> 24) & 0xFF), (std::byte)0x00,
          (std::byte)0x00, (std::byte)0x00, (std::byte)0x00, (std::byte)0x00}) {
  std::memcpy((uint8_t*)message_buffer + HEADER_SIZE, data.Get(), data_size);
}

Message::~Message() { delete[] message_buffer; }

std::byte* Message::Get() const { return message_buffer; }

std::size_t Message::Size() const { return message_size; }

}  // namespace authpp
