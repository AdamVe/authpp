#include "util.h"

#include <fmt/format.h>

#include <cstddef>

namespace authpp::util {

std::string bytesToString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        retval += fmt::format("{:02x} ", buffer.getByte(i));
    }
    retval += "]";
    retval += fmt::format("({})", buffer.size());
    return retval;
}

std::string bytesToAsciiString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        uint8_t b = buffer.getByte(i);
        if (b >= 32 && b < 127) {
            retval += b;
        } else {
            retval += '_';
        }
    }
    retval += "]";
    retval += fmt::format("({})", buffer.size());
    return retval;
}

} // namespace authpp::util
