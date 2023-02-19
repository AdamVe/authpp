#include "util.h"

#include <fmt/format.h>

#include <cstddef>

namespace authpp::util {

std::string BytesToString(const Bytes& bytes)
{
    std::string retval = "[";
    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        retval += fmt::format("{:02x} ", bytes.getByte());
    }
    retval += "]";
    retval += fmt::format("({})", bytes.size());
    return retval;
}

std::string BytesToAsciiString(const Bytes& bytes)
{
    std::string retval = "[";
    bytes.pointTo(0);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        uint8_t b = bytes.getByte();
        if (b >= 32 && b < 127) {
            retval += b;
        } else {
            retval += '_';
        }
    }
    retval += "]";
    retval += fmt::format("({})", bytes.size());
    return retval;
}

} // namespace authpp::util
