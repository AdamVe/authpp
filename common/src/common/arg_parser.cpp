#include "arg_parser.h"

#include <algorithm>

namespace authpp::common {
ArgParser::ArgParser(int argc, char** argv)
    : args(buildArgs(argc, argv))
{
}

bool ArgParser::hasParam(std::string_view value) const
{
#if __cpp_ranges_lib > 22010101L
    return std::ranges::count(args, value) > 0;
#else
    return std::find(args.cbegin(), args.cend(), std::string(value)) != args.cend();
#endif
}

std::string ArgParser::getParamValue(std::string_view value) const
{
#if __cpp_ranges_lib > 22010101L
    auto i = std::ranges::find(params, value);
#else
    auto i = std::find(args.cbegin(), args.cend(), value);
#endif
    if (i++ != args.cend() && i != args.cend()) {
        return *i;
    }
    return {};
}

ArgParser::args_t ArgParser::buildArgs(int argc, char** argv) const
{
    args_t retval;
    std::span args_span { argv, argv + argc };
    for (auto&& arg : args_span) {
        retval.emplace_back(arg);
    }
    return retval;
}
} // namespace authpp::common
