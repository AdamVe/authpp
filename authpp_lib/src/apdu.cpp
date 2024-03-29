#include "apdu.h"

namespace authpp {

Apdu::Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2)
    : Apdu(cla, ins, p1, p2, {})
{
}

Apdu::Apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, const ByteBuffer& data)
    : buffer(5 + data.size())
{
    buffer
        .setByteOrder(std::endian::little)
        .put(ByteBuffer { cla, ins, p1, p2 })
        .put<uint8_t>(data.size()) // short APDU
        .put(data);
}

const ByteBuffer& Apdu::get() const { return buffer; }

} // authpp
