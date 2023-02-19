#pragma once

#include "bytes.h"

namespace authpp {

class Apdu {
public:
    Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const Bytes& data);
    const Bytes& get() const;

private:
    Bytes bytes;
};

Apdu::Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const Bytes& data)
    : bytes(5 + data.size())
{
    bytes
        .putByte(cla)
        .putByte(ins)
        .putByte(p1)
        .putByte(p2)
        .putByte(data.size()) // short APDU
        .putBytes(data);
}

const Bytes& Apdu::get() const { return bytes; }

} // namespace authpp
