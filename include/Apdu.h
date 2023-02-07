#pragma once

#include "ByteArray.h"

namespace authpp {

template <typename T>
std::byte to_byte(T value)
{
    return static_cast<std::byte>(value) & (std::byte)0xff;
}

class Apdu {
public:
    template <typename T>
    Apdu(T cla, T ins, T p1, T p2);

    template <typename T, typename U>
    Apdu(T cla, T ins, T p1, T p2, U&& data);

    std::size_t getApduDataSize() const;

    ByteArray getApduData() const;

private:
    std::size_t apduDataSize;
    ByteArray apduData;
};

} // namespace authpp
