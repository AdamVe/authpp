#include "byte_buffer.h"

#include "fmt/fmt_byte_buffer.h"
#include "logger.h"

#include <bit>
#include <cassert>

namespace authpp {

namespace {
    Logger log("byte_buffer");
}

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

ByteBuffer& ByteBuffer::putByte(unsigned char c)
{
    if (debugLog) {
        log.d("putByte {:02x} to {} (buffer size: {})", c, pointer, data.size());
    }
    assert(pointer < data.size());
    data[pointer++] = c;

    return *this;
}

unsigned char ByteBuffer::getByte() const
{
    if (debugLog) {
        log.d("getByte from {} (buffer size: {})", pointer, data.size());
    }
    assert(pointer < data.size());
    unsigned char c = data[pointer++];
    if (debugLog) {
        log.d("getByte: {:02x}", c);
    }
    return c;
}

ByteBuffer& ByteBuffer::putShort(uint16_t i16)
{
    if (debugLog) {
        log.d("putShort {:04x} to {} (buffer size: {})", i16, pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    if (endian == std::endian::big) {
        putByte((uint8_t)(i16 >> 8));
        putByte((uint8_t)i16);
    } else {
        putByte((uint8_t)i16);
        putByte((uint8_t)(i16 >> 8));
    }
    return *this;
}

uint16_t ByteBuffer::getShort() const
{
    if (debugLog) {
        log.d("getShort from {} (buffer size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    uint16_t value = (endian == std::endian::big)
        ? data[pointer++] << 8 | data[pointer++]
        : data[pointer++] | data[pointer++] << 8;

    if (debugLog) {
        log.d("getShort: {}", value);
    }

    return value;
}

ByteBuffer& ByteBuffer::putInt(uint32_t i32)
{
    if (debugLog) {
        log.d("putInt {:08x} to {} (buffer size: {})", i32, pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    if (endian == std::endian::big) {
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

uint32_t ByteBuffer::getInt() const
{
    if (debugLog) {
        log.d("getInt from {} (buffer size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    uint32_t value = (endian == std::endian::big)
        ? data[pointer++] << 24 | data[pointer++] << 16 | data[pointer++] << 8 | data[pointer++]
        : data[pointer++] | data[pointer++] << 8 | data[pointer++] << 16 | data[pointer++];

    if (debugLog) {
        log.d("getInt: {}", value);
    }

    return value;
}

ByteBuffer& ByteBuffer::putBytes(const ByteBuffer& buffer)
{
    if (buffer.size() > 0) {

        if (debugLog) {
            log.d("putBytes {} to {} (buffer size: {})", buffer, pointer, data.size());
        }

        assert(pointer < data.size() - buffer.size() + 1);
        buffer.pointTo(0);
        for (std::size_t i = 0; i < buffer.size(); ++i) {
            auto b = buffer.getByte();
            putByte(b);
        }
    }

    return *this;
}

ByteBuffer ByteBuffer::getBytes(std::size_t size) const
{
    if (debugLog) {
        log.d("getBytes of size {} from {} (buffer size: {})", size, pointer, data.size());
    }
    assert(pointer < data.size() - size + 1);

    ByteBuffer buffer(size);
    buffer.pointTo(0);
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        auto b = getByte();
        buffer.putByte(b);
    }
    return buffer;
}

unsigned char* ByteBuffer::getRaw() const
{
    return (unsigned char*)data.data();
}

const ByteBuffer& ByteBuffer::pointTo(std::size_t i) const
{
    if (data.size() > 0) {
        assert(i < data.size());
        pointer = i;
    } else {
        pointer = 0;
    }

    return *this;
}

void ByteBuffer::setEndian(std::endian endian)
{
    ByteBuffer::endian = endian;
}

void ByteBuffer::setDebugLog(bool debugLog)
{
    ByteBuffer::debugLog = debugLog;
}

} // namespace authpp
