#pragma once

#include "byte_buffer.h"

#include <string>

namespace authpp::util {

std::string BytesToString(const ByteBuffer& buffer);

std::string BytesToAsciiString(const ByteBuffer& buffer);

} // namespace authpp::util
