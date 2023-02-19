#pragma once

#include <fmt/format.h>

#include "../byte_array.h"
#include "../util.h"

template <>
struct fmt::formatter<authpp::Bytes> {
    char presentation = 'm'; // default presentation

    template <typename FormatContext>
    auto format(const authpp::Bytes& bytes, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return presentation == 'm'
            ? fmt::format_to(ctx.out(), "{}", 123)
//                authpp::util::ByteDataToString(
//                    byteArray.Get(), byteArray.GetDataSize()))
            : fmt::format_to(ctx.out(), "{}", 456);
//                authpp::util::ByteDataToStringH(
//                    byteArray.Get(), byteArray.GetDataSize()));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'm' || *it == 'h'))
            presentation = *it++;
        return it;
    }
};
