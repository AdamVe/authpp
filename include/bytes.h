#pragma once

#include <cstddef>
#include <cstdint>

#include <initializer_list>

namespace authpp {

class Bytes {
public:
    Bytes(std::size_t size) {};

    Bytes(std::initializer_list<uint8_t> il) {};

    std::size_t size() const { return 0; }
    void setSize(std::size_t size) {}

    void pointTo(std::size_t i) {}

    void putChar(unsigned char c) {};
    void putI16(uint16_t i16) {};
    void putI32(uint32_t i32) {};
    void putString(const char* str) {};
    void putBytes(const Bytes& bytes) {};

    void getChar(unsigned char& c) const {};
    void getChar(unsigned char& c, std::size_t index) const {};
    void getI16(uint16_t& i16) const {};
    void getI16(uint16_t& i16, std::size_t index) const {};
    void getI32(uint32_t& i32) const {};
    void getString(const char** str) const {};
    void getBytes(const Bytes& bytes) const {};
    void getBytes(const Bytes& bytes, std::size_t index) const {};

    unsigned char* get_raw() const { return nullptr; }

};

} // namespace authpp
