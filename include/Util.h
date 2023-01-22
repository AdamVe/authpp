#pragma once

#include <string>

namespace authpp::util {

std::string byteDataToString(std::byte* data, std::size_t dataLength);

} // namespace authpp::util
