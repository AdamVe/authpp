#include "Util.h"

#include <cstddef>
#include <fmt/format.h>

namespace authpp::util {

std::string byteDataToString(std::byte* data, std::size_t dataLength)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < dataLength; ++i) {
        retval += fmt::format("{:02x} ", data[i]);
    }
    retval += "]";
    retval += fmt::format("({})", dataLength);
    return retval;
}

std::string byteDataToStringH(std::byte* data, std::size_t dataLength)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < dataLength; ++i) {
        auto datai = (int)(data[i]);
        if (datai >= 32 && datai < 127) {
            retval += (unsigned char)datai;
        } else {
            retval += ' ';
        }
    }
    retval += "]";
    retval += fmt::format("({})", dataLength);
    return retval;
}

} // namespace authpp::util
