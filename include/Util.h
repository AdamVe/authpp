#pragma once

#include <Byte.h>

#include <string>


namespace authpp::util {

std::string byteDataToString(byte* data, std::size_t dataLength);

} // namespace authpp::util
