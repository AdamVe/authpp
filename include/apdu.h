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
    bytes.putChar(cla);
    bytes.putChar(ins);
    bytes.putChar(p1);
    bytes.putChar(p2);
    bytes.putChar(data.size()); // short APDU
    bytes.putBytes(data);
}

const Bytes& Apdu::get() const { return bytes; }

} // namespace authpp
