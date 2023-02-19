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

    void pointTo(std::size_t i) const;

    void putChar(unsigned char c);
    void putI16(uint16_t i16);
    void putI32(uint32_t i32);
    void putBytes(const Bytes& bytes);

    void getChar(unsigned char& c) const;
    void getI16(uint16_t& i16) const;
    void getI32(uint32_t& i32) const;
    void getBytes(Bytes& bytes) const;

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
