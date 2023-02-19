#include "message.h"

#include "byte_array.h"
#include "util.h"

namespace authpp {

namespace {
    constexpr std::size_t kHeaderSize { 10 };
}

Message::Message(uint8_t type, const Bytes& data)
: bytes(kHeaderSize + data.size()) {
    bytes.putChar(type);
    bytes.putI32(data.size());
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putBytes(data);
}
//Message::Message(std::byte type, ByteArray&& data)
//    : data_size(data.GetDataSize())
//    , message_size(data_size + kHeaderSize)
//    , message_buffer(new std::byte[message_size] {
//          type, (std::byte)((data_size >> 0) & 0xFF),
//          (std::byte)((data_size >> 8) & 0xFF),
//          (std::byte)((data_size >> 16) & 0xFF),
//          (std::byte)((data_size >> 24) & 0xFF), (std::byte)0x00,
//          (std::byte)0x00, (std::byte)0x00, (std::byte)0x00, (std::byte)0x00 })
//{
//    std::memcpy((uint8_t*)message_buffer + kHeaderSize, data.Get(), data_size);
//}

Message::~Message() { }
//
const Bytes& Message::get() const { return bytes; }
//
//std::size_t Message::Size() const { return message_size; }

} // namespace authpp
