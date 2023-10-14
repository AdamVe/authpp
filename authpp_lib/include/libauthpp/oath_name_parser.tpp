#include <regex>

namespace authpp::oath {

auto OathNameParser::parseTotpName(const std::string& data)
{
    std::string name;
    std::string issuer;
    int timeStep { Credential::DEFAULT_TIME_STEP };

    static const std::regex regex(R"(^(([^/]*)\/)*([^:\n]*)(:(.*))*)");
    std::smatch matches;

    if (data.empty() || !std::regex_match(data, matches, regex) || matches.size() < 5) {
        return std::make_tuple<>(std::string("invalid"), std::string(""), 30);
    }

    if (matches[1].matched) {
        try {
            timeStep = std::stoi(matches[1].str());
        } catch (...) {
            // ignored
        }
    }

    if (!matches[5].matched && matches[3].matched) {
        name = matches[3].str();
    } else if (matches[5].matched && matches[3].matched) {
        name = matches[5].str();
        issuer = matches[3].str();
    }
    return std::make_tuple<>(name, issuer, timeStep);
}

} // authpp::oath
