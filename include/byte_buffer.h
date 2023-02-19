#pragma once

#include <cstddef>
#include <cstdint>

#include <bit>
#include <initializer_list>
#include <vector>

namespace authpp {

class Bytes {
public:
    Bytes(std::size_t size);

    Bytes(std::initializer_list<uint8_t> il);

    std::size_t size() const;
    void setSize(std::size_t size);

    const Bytes& pointTo(std::size_t i) const;

    Bytes& putByte(unsigned char c);
    Bytes& putShort(uint16_t i16);
    Bytes& putInt(uint32_t i32);
    Bytes& putBytes(const Bytes& bytes);

    uint8_t getByte() const;
    uint16_t getShort() const;
    uint32_t getInt() const;
    Bytes getBytes(std::size_t size) const;

    unsigned char* getRaw() const;

    static void setEndian(std::endian);
    static void setDebugLog(bool);

private:
    mutable std::size_t pointer = 0;
    std::vector<uint8_t> data;

    static inline std::endian endian { std::endian::little };
    static inline bool debugLog { false };
};

} // namespace authpp
