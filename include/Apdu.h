#pragma once

#include <ByteArray.h>
#include <Logger.h>
#include <Util.h>

namespace authpp {

class Apdu {
public:
    template <typename T, typename U>
    Apdu(T cla, T ins, T p1, T p2, U&& data)
        : cla((std::byte)(cla & (T)0xff))
        , ins((std::byte)(ins & (T)0xff))
        , p1((std::byte)(p1 & (T)0xff))
        , p2((std::byte)(p2 & (T)0xff))
        , data(data)
        , apduDataSize(5 + data.getSize())
        , apduData(apduDataSize)
    {
        apduData.get()[0] = (std::byte)cla;
        apduData.get()[1] = (std::byte)ins;
        apduData.get()[2] = (std::byte)p1;
        apduData.get()[3] = (std::byte)p2;
        apduData.get()[4] = (std::byte)data.getSize(); // short APDU

        Log.d("Building APDU [{:02x} {:02x} {:02x} {:02x}] with {}", (int)cla, (int)ins, (int)p1, (int)p2, util::byteDataToString(data.get(), data.getSize()));

        std::memcpy(apduData.get() + 5, data.get(), data.getSize());
    }

    std::size_t getApduDataSize() const
    {
        return apduDataSize;
    }

    ByteArray getApduData() const
    {
        return apduData;
    }

private:
    std::byte cla;
    std::byte ins;
    std::byte p1;
    std::byte p2;
    std::byte len;
    ByteArray data;
    std::size_t apduDataSize;
    ByteArray apduData;
};

} // namespace authpp
