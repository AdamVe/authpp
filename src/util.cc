#include "util.h"

#include <fmt/format.h>

#include <cstddef>

namespace authpp::util {

std::string BytesToString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    buffer.pointTo(0);
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        retval += fmt::format("{:02x} ", buffer.getByte());
    }
    retval += "]";
    retval += fmt::format("({})", buffer.size());
    return retval;
}

std::string BytesToAsciiString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    buffer.pointTo(0);
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        uint8_t b = buffer.getByte();
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
