#pragma once

#include <string>

namespace authpp::util {

std::string ByteDataToString(std::byte* data, std::size_t dataLength);

std::string ByteDataToStringH(std::byte* data, std::size_t dataLength);

}  // namespace authpp::util
