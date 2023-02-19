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
        uint8(value);
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

Bytes& Bytes::uint8(unsigned char c)
{
    if (debugLog) {
        log.d("uint8 {:02x} to {} (bytes size: {})", c, pointer, data.size());
    }
    assert(pointer < data.size());
    data[pointer++] = c;

    return *this;
}

unsigned char Bytes::uint8() const
{
    if (debugLog) {
        log.d("uint8 from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size());
    unsigned char c = data[pointer++];
    if (debugLog) {
        log.d("uint8: {:02x}", c);
    }
    return c;
}

Bytes& Bytes::uint16(uint16_t i16)
{
    if (debugLog) {
        log.d("putI16 {:04x} to {} (bytes size: {})", i16, pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    if (endian == std::endian::big) {
        uint8((uint8_t)(i16 >> 8));
        uint8((uint8_t)i16);
    } else {
        uint8((uint8_t)i16);
        uint8((uint8_t)(i16 >> 8));
    }
    return *this;
}

uint16_t Bytes::uint16() const
{
    if (debugLog) {
        log.d("uint16 from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 1);
    uint16_t value = (endian == std::endian::big)
        ? data[pointer++] << 8 | data[pointer++]
        : data[pointer++] | data[pointer++] << 8;

    if (debugLog) {
        log.d("uint16: {}", value);
    }

    return value;
}

Bytes& Bytes::uint32(uint32_t i32)
{
    if (debugLog) {
        log.d("putI32 {:08x} to {} (bytes size: {})", i32, pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    if (endian == std::endian::big) {
        uint8((uint8_t)(i32 >> 24));
        uint8((uint8_t)(i32 >> 16));
        uint8((uint8_t)(i32 >> 8));
        uint8((uint8_t)(i32));
    } else {
        uint8((uint8_t)(i32));
        uint8((uint8_t)(i32 >> 8));
        uint8((uint8_t)(i32 >> 16));
        uint8((uint8_t)(i32 >> 24));
    }

    return *this;
}

uint32_t Bytes::uint32() const
{
    if (debugLog) {
        log.d("uint32 from {} (bytes size: {})", pointer, data.size());
    }
    assert(pointer < data.size() - 3);
    uint32_t value = (endian == std::endian::big)
        ? data[pointer++] << 24 | data[pointer++] << 16 | data[pointer++] << 8 | data[pointer++]
        : data[pointer++] | data[pointer++] << 8 | data[pointer++] << 16 | data[pointer++];

    if (debugLog) {
        log.d("uint32: {}", value);
    }

    return value;
}

Bytes& Bytes::set(const Bytes& bytes)
{
    if (bytes.size() > 0) {

        if (debugLog) {
            log.d("set {} to {} (bytes size: {})", bytes, pointer, data.size());
        }

        assert(pointer < data.size() - bytes.size() + 1);
        bytes.pointTo(0);
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            auto b = bytes.uint8();
            uint8(b);
        }
    }

    return *this;
}

Bytes Bytes::get(std::size_t size) const
{
    if (debugLog) {
        log.d("get of size {} from {} (bytes size: {})", size, pointer, data.size());
    }
    assert(pointer < data.size() - size + 1);

    Bytes bytes(size);
    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        auto b = uint8();
        bytes.uint8(b);
    }
    return bytes;
}

unsigned char* Bytes::getRaw() const
{
    return (unsigned char*)data.data();
}

const Bytes& Bytes::pointTo(std::size_t i) const
{
    if (data.size() >= 0) {
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
