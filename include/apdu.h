#pragma once

#include "byte_buffer.h"

namespace authpp {

class Apdu {
public:

    Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2);
    Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const ByteBuffer& data);
    const ByteBuffer& get() const;

private:
    ByteBuffer buffer;
};

} // namespace authpp
