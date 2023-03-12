#pragma once

#include <cstddef>
#include <cstdint>

#include <bit>
#include <initializer_list>
#include <vector>

namespace authpp {

class ByteBuffer {
public:
    ByteBuffer();

    explicit ByteBuffer(std::size_t size);

    explicit ByteBuffer(std::initializer_list<uint8_t> il);

    std::size_t size() const;
    void setSize(std::size_t size);

    const ByteBuffer& pointTo(std::size_t i);

    ByteBuffer& setByteOrder(std::endian);

    ByteBuffer& putByte(unsigned char c);
    ByteBuffer& putShort(uint16_t i16);
    ByteBuffer& putInt(uint32_t i32);
    ByteBuffer& putLong(uint64_t i64);
    ByteBuffer& putBytes(const ByteBuffer& buffer);

    uint8_t getByte(std::size_t index) const;
    uint16_t getShort(std::size_t index) const;
    uint32_t getInt(std::size_t index) const;

    ByteBuffer getBytes(std::size_t from_index, std::size_t size) const;

    uint8_t* array() const;

    bool operator==(const ByteBuffer& other) const;

    static void setDebugLog(bool);

private:
    std::endian byteOrder { std::endian::big };
    std::size_t pointer = 0;
    std::vector<uint8_t> data;

    static inline bool debugLog { false };
};

} // namespace authpp
