#include "message.h"

#include "byte_buffer.h"

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
        .put<uint8_t>(type)
        .put<uint32_t>(data.size())
        .put(ByteBuffer { 0x00, 0x00, 0x00, 0x00, 0x00 })
        .put(data);
}

Message::~Message() = default;

const ByteBuffer& Message::get() const { return buffer; }

} // namespace authpp
