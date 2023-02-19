#include "message.h"

namespace authpp {

namespace {
    constexpr std::size_t kHeaderSize { 10 };
}

Message::Message(uint8_t type, const Bytes& data)
    : bytes(kHeaderSize + data.size())
{
    bytes.putChar(type);
    bytes.putI32(data.size());
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putChar(0x00);
    bytes.putBytes(data);
}

Message::~Message() = default;
//
const Bytes& Message::get() const { return bytes; }

} // namespace authpp
