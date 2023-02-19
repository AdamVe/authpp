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
        putChar(value);
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

void Bytes::putChar(unsigned char c)
{
    if (debugLog) {
        log.d("putChar {:02x} to {} (bytes size: {})", c, pointer, data.size());
    }
    assert(pointer < data.size());
    data[pointer++] = c;
}

void Bytes::getChar(unsigned char& c) const
{
    if (debugLog) {
        log.d("getChar from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size());
    c = data[pointer++];
    if (debugLog) {
        log.d("getChar: {:02x}", c);
    }
}

void Bytes::putI16(uint16_t i16)
{
    if (debugLog) {
        log.d("putI16 {:04x} to {} (bytes size: {})", i16, pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    if (endian == std::endian::big) {
        putChar((uint8_t)(i16 >> 8));
        putChar((uint8_t)i16);
    } else {
        putChar((uint8_t)i16);
        putChar((uint8_t)(i16 >> 8));
    }
}

void Bytes::getI16(uint16_t& i16) const
{
    if (debugLog) {
        log.d("getI16 from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    if (endian == std::endian::big) {
        i16 = data[pointer++] << 8 | data[pointer++];
    } else {
        i16 = data[pointer++] | data[pointer++] << 8;
    }
    if (debugLog) {
        log.d("getI16: {}", i16);
    }
}

void Bytes::putI32(uint32_t i32)
{
    if (debugLog) {
        log.d("putI32 {:08x} to {} (bytes size: {})", i32, pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    if (endian == std::endian::big) {
        putChar((uint8_t)(i32 >> 24));
        putChar((uint8_t)(i32 >> 16));
        putChar((uint8_t)(i32 >> 8));
        putChar((uint8_t)(i32));
    } else {
        putChar((uint8_t)(i32));
        putChar((uint8_t)(i32 >> 8));
        putChar((uint8_t)(i32 >> 16));
        putChar((uint8_t)(i32 >> 24));
    }
}

void Bytes::getI32(uint32_t& i32) const
{
    if (debugLog) {
        log.d("getI32 from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    if (endian == std::endian::big) {
        i32 = data[pointer++] << 24 | data[pointer++] << 16 | data[pointer++] << 8 | data[pointer++];
    } else {
        i32 = data[pointer++] | data[pointer++] << 8 | data[pointer++] << 16 | data[pointer++];
    }

    if (debugLog) {
        log.d("getI32: {}", i32);
    }
}

void Bytes::putBytes(const Bytes& bytes)
{
    if (bytes.size() == 0) {
        return;
    }

    if (debugLog) {
        log.d("putBytes {} to {} (bytes size: {})", bytes, pointer, data.size());
    }

    assert(pointer < data.size() - bytes.size() + 1);
    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        uint8_t b;
        bytes.getChar(b);
        putChar(b);
    }
}

void Bytes::getBytes(Bytes& bytes) const
{
    if (debugLog) {
        log.d("getBytes of size {} from {} (bytes size: {})", bytes.size(), pointer, data.size());
    }
    assert(pointer < data.size() - bytes.size() + 1);

    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        uint8_t b;
        getChar(b);
        bytes.putChar(b);
    }
}

unsigned char* Bytes::getRaw() const
{
    return (unsigned char*)data.data();
}

void Bytes::pointTo(std::size_t i) const
{
    if (data.size() == 0) {
        pointer = 0;
        return;
    }

    assert(i < data.size());
    pointer = i;
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
