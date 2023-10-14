#pragma once

#include "byte_buffer.h"

#include <string>

namespace authpp::util {

std::string bytesToString(const ByteBuffer& buffer);

std::string bytesToAsciiString(const ByteBuffer& buffer);

} // authpp::util
