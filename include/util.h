#pragma once

#include "bytes.h"

#include <string>

namespace authpp::util {

std::string BytesToString(const Bytes& bytes);

std::string BytesToAsciiString(const Bytes& bytes);

} // namespace authpp::util
