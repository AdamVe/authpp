#pragma once

#include <fmt/format.h>

#include "../byte_array.h"
#include "../util.h"

template <>
struct fmt::formatter<authpp::ByteArray> {
    char presentation = 'm'; // default presentation

    template <typename FormatContext>
    auto format(const authpp::ByteArray& byteArray, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return presentation == 'm'
            ? fmt::format_to(ctx.out(), "{}",
                authpp::util::ByteDataToString(
                    byteArray.Get(), byteArray.GetDataSize()))
            : fmt::format_to(ctx.out(), "{}",
                authpp::util::ByteDataToStringH(
                    byteArray.Get(), byteArray.GetDataSize()));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'm' || *it == 'h'))
            presentation = *it++;
        return it;
    }
};
