#pragma once

#include "oath_session.h"

#include <string>

namespace authpp::oath {
class OathNameParser {
public:
    static auto parseTotpName(const std::string& data);
};

} // authppgtk::oath

#include "oath_name_parser.tpp"