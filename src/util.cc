#include "util.h"

#include <fmt/format.h>

#include <cstddef>

namespace authpp::util {

std::string ByteDataToString(std::byte* data, std::size_t length) {
  std::string retval = "[";
  for (std::size_t i = 0; i < length; ++i) {
    retval += fmt::format("{:02x} ", data[i]);
  }
  retval += "]";
  retval += fmt::format("({})", length);
  return retval;
}

std::string ByteDataToStringH(std::byte* data, std::size_t length) {
  std::string retval = "[";
  for (std::size_t i = 0; i < length; ++i) {
    auto datai = (int)(data[i]);
    if (datai >= 32 && datai < 127) {
      retval += (unsigned char)datai;
    } else {
      retval += ' ';
    }
  }
  retval += "]";
  retval += fmt::format("({})", length);
  return retval;
}

}  // namespace authpp::util
