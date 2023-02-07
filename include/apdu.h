#pragma once

#include "byte_array.h"

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
    std::size_t data_size;
    ByteArray data;
};

template <typename T>
Apdu::Apdu(T cla, T ins, T p1, T p2)
    : Apdu(cla, ins, p1, p2, ByteArray(0))
{
}

template <typename T, typename U>
Apdu::Apdu(T cla, T ins, T p1, T p2, U&& apdu_data)
    : data_size(5 + apdu_data.getDataSize())
    , data(data_size)
{
    data.get()[0] = to_byte(cla);
    data.get()[1] = to_byte(ins);
    data.get()[2] = to_byte(p1);
    data.get()[3] = to_byte(p2);

    data.get()[4] = (std::byte)apdu_data.getDataSize(); // short APDU
    std::memcpy(data.get() + 5, apdu_data.get(), apdu_data.getDataSize());
}

std::size_t Apdu::getApduDataSize() const
{
    return data_size;
}

ByteArray Apdu::getApduData() const
{
    return data;
}

} // namespace authpp
