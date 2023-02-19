#pragma once

#include <cstddef>
#include <cstdint>

#include <bit>
#include <initializer_list>
#include <vector>

namespace authpp {

class ByteBuffer {
public:
    ByteBuffer(std::size_t size);

    ByteBuffer(std::initializer_list<uint8_t> il);

    std::size_t size() const;
    void setSize(std::size_t size);

    const ByteBuffer& pointTo(std::size_t i) const;

    ByteBuffer& putByte(unsigned char c);
    ByteBuffer& putShort(uint16_t i16);
    ByteBuffer& putInt(uint32_t i32);
    ByteBuffer& putBytes(const ByteBuffer& buffer);

    uint8_t getByte() const;
    uint16_t getShort() const;
    uint32_t getInt() const;
    ByteBuffer getBytes(std::size_t size) const;

    uint8_t* array() const;

    static void setEndian(std::endian);
    static void setDebugLog(bool);

private:
    mutable std::size_t pointer = 0;
    std::vector<uint8_t> data;

    static inline std::endian endian { std::endian::little };
    static inline bool debugLog { false };
};

} // namespace authpp
