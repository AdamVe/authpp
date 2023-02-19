#pragma once

#include <fmt/format.h>

#include <util.h>

using namespace authpp::util;

template <>
struct fmt::formatter<authpp::Bytes> {
    char presentation = 'm'; // default presentation

    template <typename FormatContext>
    auto format(const authpp::Bytes& bytes, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return presentation == 'm'
            ? fmt::format_to(ctx.out(), "{}", BytesToString(bytes))
            : fmt::format_to(ctx.out(), "{}", BytesToAsciiString(bytes));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'm' || *it == 'h'))
            presentation = *it++;
        return it;
    }
};
