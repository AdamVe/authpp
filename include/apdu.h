#pragma once

#include "byte_array.h"
#include "logger.h"

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

namespace internal {
    Logger log("Apdu");
}

template <typename T>
Apdu::Apdu(T cla, T ins, T p1, T p2)
    : Apdu(cla, ins, p1, p2, ByteArray(0))
{
}

template <typename T, typename U>
Apdu::Apdu(T cla, T ins, T p1, T p2, U&& data)
    : apduDataSize(5 + data.getDataSize())
    , apduData(apduDataSize)
{
    apduData.get()[0] = to_byte(cla);
    apduData.get()[1] = to_byte(ins);
    apduData.get()[2] = to_byte(p1);
    apduData.get()[3] = to_byte(p2);

    apduData.get()[4] = (std::byte)data.getDataSize(); // short APDU

    internal::log.d("Building APDU [{:02x} {:02x} {:02x} {:02x}] with {}", cla, ins, p1, p2, data);

    std::memcpy(apduData.get() + 5, data.get(), data.getDataSize());
}

std::size_t Apdu::getApduDataSize() const
{
    return apduDataSize;
}

ByteArray Apdu::getApduData() const
{
    return apduData;
}

} // namespace authpp
