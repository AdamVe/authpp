#include <Util.h>

#include <fmt/format.h>

namespace authpp::util {

std::string byteDataToString(std::byte* data, std::size_t dataLength)
{
    std::string retval = "[";
    for (std::size_t i = 0; i < dataLength; ++i) {
        retval += fmt::format("{:02x} ", data[i]);
    }
    retval += "]";
    return retval;
}

} // namespace authpp::util
