#include "byte_buffer.h"

#include "fmt/fmt_byte_buffer.h"
#include "logger.h"

#include <bit>
#include <cassert>

namespace authpp {

namespace {
    Logger log("byte_buffer");
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
    if (data.size() > 0) {
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

void ByteBuffer::setDebugLog(bool debugLog)
{
    ByteBuffer::debugLog = debugLog;
}

} // namespace authpp
