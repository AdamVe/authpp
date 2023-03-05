#include "message.h"

namespace authpp {

namespace {
    constexpr std::size_t kHeaderSize { 10 };
}

Message::Message(uint8_t type, const ByteBuffer& data)
    : buffer(kHeaderSize + data.size())
{
    ;
    buffer
        .setByteOrder(std::endian::little)
        .putByte(type)
        .putInt(data.size())
        .putBytes(ByteBuffer { 0x00, 0x00, 0x00, 0x00, 0x00 })
        .putBytes(data);
}

Message::~Message() = default;

const ByteBuffer& Message::get() const { return buffer; }

} // namespace authpp
