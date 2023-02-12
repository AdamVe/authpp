#pragma once

#include <string>

namespace authpp::util {

std::string ByteDataToString(std::byte* data, std::size_t length);

std::string ByteDataToStringH(std::byte* data, std::size_t length);

} // namespace authpp::util
