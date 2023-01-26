#pragma once

#include <ByteArray.h>

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
    {
    }

private:
    std::byte cla;
    std::byte ins;
    std::byte p1;
    std::byte p2;
    ByteArray data;
};

} // namespace authpp
