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

    Bytes& uint8(unsigned char c);
    Bytes& uint16(uint16_t i16);
    Bytes& uint32(uint32_t i32);
    Bytes& set(const Bytes& bytes);

    uint8_t uint8() const;
    uint16_t uint16() const;
    uint32_t uint32() const;
    Bytes get(std::size_t size) const;

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
