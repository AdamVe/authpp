#include "message.h"

namespace authpp {

namespace {
    constexpr std::size_t kHeaderSize { 10 };
}

Message::Message(uint8_t type, const Bytes& data)
    : bytes(kHeaderSize + data.size())
{
    bytes
        .uint8(type)
        .uint32(data.size())
        .uint8(0x00)
        .uint8(0x00)
        .uint8(0x00)
        .uint8(0x00)
        .uint8(0x00)
        .set(data);
}

Message::~Message() = default;
//
const Bytes& Message::get() const { return bytes; }

} // namespace authpp
