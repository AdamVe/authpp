#include "byte_buffer.h"

#include "fmt/fmt_byte_buffer.h"
#include "logger.h"

#include <bit>
#include <cassert>

namespace authpp {

namespace {
    Logger log("byte_buffer");
}

template<>
uint8_t byteswap(uint8_t b)
{
    return b;
}

template<>
uint16_t byteswap(uint16_t b)
{
#if __cpp_lib_byteswap >= 202110L
    return std::byteswap(b);
#else
    return (b & 0x00ff) << 8 | (b & 0xff00) >> 8;
#endif
}

template<>
uint32_t byteswap(uint32_t b)
{
#if __cpp_lib_byteswap >= 202110L
    return std::byteswap(b);
#else
    return
        (b & 0x000000ff) << 24 |
        (b & 0x0000ff00) << 8 |
        (b & 0x00ff0000) >> 8 |
        (b & 0xff000000) >> 24;
#endif
}

template<>
uint64_t byteswap(uint64_t b)
{
#if __cpp_lib_byteswap >= 202110L
    return std::byteswap(b);
#else
    return
        (b & 0x00000000000000ff) << 56 |
        (b & 0x000000000000ff00) << 40 |
        (b & 0x0000000000ff0000) << 24 |
        (b & 0x00000000ff000000) << 8 |
        (b & 0x000000ff00000000) >> 8 |
        (b & 0x0000ff0000000000) >> 24 |
        (b & 0x00ff000000000000) >> 40 |
        (b & 0xff00000000000000) >> 56;
#endif
}


ByteBuffer::ByteBuffer() = default;

ByteBuffer::ByteBuffer(std::size_t size)
    : data(size)
{
}

ByteBuffer::ByteBuffer(std::initializer_list<uint8_t> il)
    : data(il.size())
{
    for (auto&& value : il) {
        put(value);
    }
    pointTo(0);
}

void ByteBuffer::setSize(std::size_t size)
{
    data.resize(size);
}

std::size_t ByteBuffer::size() const
{
    return data.size();
}

ByteBuffer& ByteBuffer::setByteOrder(std::endian order)
{
    byteOrder = order;
    return *this;
}

ByteBuffer& ByteBuffer::put(const ByteBuffer& buffer)
{
    auto bs = buffer.size();
    assert(pointer < data.size() - bs + 1);
    std::memcpy(data.data() + pointer, buffer.array(), bs);
    pointer += bs;
    return *this;
}

ByteBuffer ByteBuffer::get(std::size_t index, std::size_t size) const
{
    if (debugLog) {
        log.d("getBytes from {} of size {} from {} (buffer size: {})", index, size, *this, data.size());
    }
    assert(index < data.size() - size + 1);

    ByteBuffer buffer(size);
    std::memcpy(buffer.array(), data.data() + index, size);
    buffer.pointTo(size - 1);
    return buffer;
}

uint8_t* ByteBuffer::array() const
{
    return (uint8_t*)data.data();
}

const ByteBuffer& ByteBuffer::pointTo(std::size_t index)
{
    if (!data.empty()) {
        assert(index < data.size());
        pointer = index;
    } else {
        pointer = 0;
    }

    return *this;
}

bool ByteBuffer::operator==(const ByteBuffer& other) const
{
    return data == other.data;
}

void ByteBuffer::setDebugLog(bool enabled)
{
    ByteBuffer::debugLog = enabled;
}

} // namespace authpp
