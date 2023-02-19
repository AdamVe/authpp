#include "message.h"

namespace authpp {

namespace {
    constexpr std::size_t kHeaderSize { 10 };
}

Message::Message(uint8_t type, const Bytes& data)
    : bytes(kHeaderSize + data.size())
{
    bytes
        .putByte(type)
        .putInt(data.size())
        .putByte(0x00)
        .putByte(0x00)
        .putByte(0x00)
        .putByte(0x00)
        .putByte(0x00)
        .putBytes(data);
}

Message::~Message() = default;
//
const Bytes& Message::get() const { return bytes; }

} // namespace authpp
