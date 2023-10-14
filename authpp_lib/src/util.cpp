#include "util.h"

#include <fmt/format.h>

namespace authpp::util {

std::string bytesToString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        retval += fmt::format("{:02x} ", buffer.get<uint8_t>(i));
    }
    retval += "]";
    retval += fmt::format("({})", buffer.size());
    return retval;
}

std::string bytesToAsciiString(const ByteBuffer& buffer)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        std::integral auto b = buffer.get<uint8_t>(i);
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

} // authpp::util
