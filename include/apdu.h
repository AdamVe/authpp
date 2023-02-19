#pragma once

#include "byte_buffer.h"

namespace authpp {

class Apdu {
public:
    Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const ByteBuffer& data);
    const ByteBuffer& get() const;

private:
    ByteBuffer buffer;
};

Apdu::Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const ByteBuffer& data)
    : buffer(5 + data.size())
{
    buffer
        .putByte(cla)
        .putByte(ins)
        .putByte(p1)
        .putByte(p2)
        .putByte(data.size()) // short APDU
        .putBytes(data);
}

const ByteBuffer& Apdu::get() const { return buffer; }

} // namespace authpp
