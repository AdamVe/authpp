#pragma once

#include <fmt/format.h>

#include <util.h>

using namespace authpp::util;

template <>
struct fmt::formatter<authpp::ByteBuffer> {
    char presentation = 'm'; // default presentation

    template <typename FormatContext>
    auto format(const authpp::ByteBuffer& buffer, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return presentation == 'm'
            ? fmt::format_to(ctx.out(), "{}", bytesToString(buffer))
            : fmt::format_to(ctx.out(), "{}", bytesToAsciiString(buffer));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'm' || *it == 'h'))
            presentation = *it++;
        return it;
    }
};
