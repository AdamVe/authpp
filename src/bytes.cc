#include "bytes.h"

#include "fmt/fmt_bytes.h"
#include "logger.h"

#include <bit>
#include <cassert>

namespace authpp {

namespace {
    Logger log("bytes");
}

Bytes::Bytes(std::size_t size)
    : data(size)
{
}

Bytes::Bytes(std::initializer_list<uint8_t> il)
    : data(il.size())
{
    for (auto&& value : il) {
        putByte(value);
    }
    pointTo(0);
}

void Bytes::setSize(std::size_t size)
{
    data.resize(size);
}

std::size_t Bytes::size() const
{
    return data.size();
}

Bytes& Bytes::putByte(unsigned char c)
{
    if (debugLog) {
        log.d("putByte {:02x} to {} (bytes size: {})", c, pointer, data.size());
    }
    assert(pointer < data.size());
    data[pointer++] = c;

    return *this;
}

unsigned char Bytes::getByte() const
{
    if (debugLog) {
        log.d("getByte from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size());
    unsigned char c = data[pointer++];
    if (debugLog) {
        log.d("getByte: {:02x}", c);
    }
    return c;
}

Bytes& Bytes::putShort(uint16_t i16)
{
    if (debugLog) {
        log.d("putShort {:04x} to {} (bytes size: {})", i16, pointer, data.size());
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

uint16_t Bytes::getShort() const
{
    if (debugLog) {
        log.d("getShort from {} (bytes size: {})", pointer, data.size());
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

Bytes& Bytes::putInt(uint32_t i32)
{
    if (debugLog) {
        log.d("putInt {:08x} to {} (bytes size: {})", i32, pointer, data.size());
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

uint32_t Bytes::getInt() const
{
    if (debugLog) {
        log.d("getInt from {} (bytes size: {})", pointer, data.size());
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

Bytes& Bytes::putBytes(const Bytes& bytes)
{
    if (bytes.size() > 0) {

        if (debugLog) {
            log.d("putBytes {} to {} (bytes size: {})", bytes, pointer, data.size());
        }

        assert(pointer < data.size() - bytes.size() + 1);
        bytes.pointTo(0);
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            auto b = bytes.getByte();
            putByte(b);
        }
    }

    return *this;
}

Bytes Bytes::getBytes(std::size_t size) const
{
    if (debugLog) {
        log.d("getBytes of size {} from {} (bytes size: {})", size, pointer, data.size());
    }
    assert(pointer < data.size() - size + 1);

    Bytes bytes(size);
    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        auto b = getByte();
        bytes.putByte(b);
    }
    return bytes;
}

unsigned char* Bytes::getRaw() const
{
    return (unsigned char*)data.data();
}

const Bytes& Bytes::pointTo(std::size_t i) const
{
    if (data.size() > 0) {
        assert(i < data.size());
        pointer = i;
    } else {
        pointer = 0;
    }

    return *this;
}

void Bytes::setEndian(std::endian endian)
{
    Bytes::endian = endian;
}

void Bytes::setDebugLog(bool debugLog)
{
    Bytes::debugLog = debugLog;
}

} // namespace authpp
