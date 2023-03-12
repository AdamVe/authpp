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
        putByte(value);
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

ByteBuffer& ByteBuffer::putByte(unsigned char c)
{
    if (debugLog) {
        log.d("putByte {:02x} to {} (buffer size: {})", c, pointer, data.size());
    }
    assert(pointer < data.size());
    data[pointer++] = c;

    return *this;
}

ByteBuffer& ByteBuffer::putShort(uint16_t i16)
{
    if (debugLog) {
        log.d("putShort {:04x} to {} (buffer size: {})", i16, pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    if (byteOrder == std::endian::big) {
        putByte((uint8_t)(i16 >> 8));
        putByte((uint8_t)i16);
    } else {
        putByte((uint8_t)i16);
        putByte((uint8_t)(i16 >> 8));
    }
    return *this;
}

ByteBuffer& ByteBuffer::putInt(uint32_t i32)
{
    if (debugLog) {
        log.d("putInt {:08x} to {} (buffer size: {})", i32, pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    if (byteOrder == std::endian::big) {
        putByte((uint8_t)(i32 >> 24));
        putByte((uint8_t)(i32 >> 16));
        putByte((uint8_t)(i32 >> 8));
        putByte((uint8_t)(i32));
    } else {
        putByte((uint8_t)(i32));
        putByte((uint8_t)(i32 >> 8));
        putByte((uint8_t)(i32 >> 16));
        putByte((uint8_t)(i32 >> 24));
    }

    return *this;
}

ByteBuffer& ByteBuffer::putLong(uint64_t i64)
{
    if (debugLog) {
        log.d("putLong {:016x} to {} (buffer size: {})", i64, pointer, data.size());
    }
    assert(pointer < data.size() - 7);
    if (byteOrder == std::endian::big) {
        putByte((uint8_t)(i64 >> 56));
        putByte((uint8_t)(i64 >> 48));
        putByte((uint8_t)(i64 >> 40));
        putByte((uint8_t)(i64 >> 32));
        putByte((uint8_t)(i64 >> 24));
        putByte((uint8_t)(i64 >> 16));
        putByte((uint8_t)(i64 >> 8));
        putByte((uint8_t)(i64));
    } else {
        putByte((uint8_t)(i64));
        putByte((uint8_t)(i64 >> 8));
        putByte((uint8_t)(i64 >> 16));
        putByte((uint8_t)(i64 >> 24));
        putByte((uint8_t)(i64 >> 32));
        putByte((uint8_t)(i64 >> 40));
        putByte((uint8_t)(i64 >> 48));
        putByte((uint8_t)(i64 >> 56));
    }

    if (debugLog) {
        log.d("Stored long {}", i64);
    }
    return *this;
}

ByteBuffer& ByteBuffer::putBytes(const ByteBuffer& buffer)
{
    if (buffer.size() > 0) {

        if (debugLog) {
            log.d("putBytes {} to {} (buffer size: {})", buffer, pointer, data.size());
        }

        assert(pointer < data.size() - buffer.size() + 1);
        for (std::size_t i = 0; i < buffer.size(); ++i) {
            auto b = buffer.getByte(i);
            putByte(b);
        }
    }

    return *this;
}

unsigned char ByteBuffer::getByte(std::size_t i) const
{
    if (debugLog) {
        log.d("getByte from {} (buffer size: {})", i, data.size());
    }
    assert(i < data.size());
    unsigned char c = data[i];
    if (debugLog) {
        log.d("getByte: {:02x}", c);
    }
    return c;
}

uint16_t ByteBuffer::getShort(std::size_t i) const
{
    if (debugLog) {
        log.d("getShort from {} (buffer size: {})", i, data.size());
    }
    assert(i < data.size() - 1);
    uint16_t value = byteOrder == std::endian::big
        ? data[i] << 8 | data[i + 1]
        : data[i] | data[i + 1] << 8;

    if (debugLog) {
        log.d("getShort: {}", value);
    }

    return value;
}

uint32_t ByteBuffer::getInt(std::size_t i) const
{
    if (debugLog) {
        log.d("getInt from {} (buffer size: {})", i, data.size());
    }
    assert(i < data.size() - 3);
    uint32_t value = byteOrder == std::endian::big
        ? (data[i] << 24) + (data[i + 1] << 16) + (data[i + 2] << 8) + data[i + 3]
        : data[i] + (data[i + 1] << 8) + (data[i + 2] << 16) + (data[i + 3] << 24);

    if (debugLog) {
        log.d("getInt({}): {}", i, value);
    }

    return value;
}

ByteBuffer ByteBuffer::getBytes(std::size_t from_index, std::size_t size) const
{
    if (debugLog) {
        log.d("getBytes from {} of size {} from {} (buffer size: {})", from_index, size, *this, data.size());
    }
    assert(from_index < data.size() - size + 1);

    ByteBuffer buffer(size);
    for (std::size_t i = from_index; i < from_index + buffer.size(); ++i) {
        auto b = getByte(i);
        buffer.putByte(b);
    }
    return buffer;
}

uint8_t* ByteBuffer::array() const
{
    return (uint8_t*)data.data();
}

const ByteBuffer& ByteBuffer::pointTo(std::size_t i)
{
    if (data.size() > 0) {
        assert(i < data.size());
        pointer = i;
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
