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
        .uint8(cla)
        .uint8(ins)
        .uint8(p1)
        .uint8(p2)
        .uint8(data.size()) // short APDU
        .set(data);
}

const Bytes& Apdu::get() const { return bytes; }

} // namespace authpp
