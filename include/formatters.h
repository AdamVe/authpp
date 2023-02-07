#pragma once

#include "byte_array.h"
#include "logger.h"
#include "message.h"
#include "util.h"

#include <fmt/format.h>

template <>
struct fmt::formatter<authpp::Logger::Level> {
    template <typename FormatContext>
    auto format(const authpp::Logger::Level& level, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", std::to_underlying(level));
    }
};

template <>
struct fmt::formatter<authpp::ByteArray> {

    char presentation = 'm'; // default presentation

    template <typename FormatContext>
    auto format(const authpp::ByteArray& byteArray, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return presentation == 'm'
            ? fmt::format_to(ctx.out(), "{}", authpp::util::byteDataToString(byteArray.Get(), byteArray.GetDataSize()))
            : fmt::format_to(ctx.out(), "{}", authpp::util::byteDataToStringH(byteArray.Get(), byteArray.GetDataSize()));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'm' || *it == 'h'))
            presentation = *it++;
        return it;
    }
};

template <>
struct fmt::formatter<authpp::Message> {
    template <typename FormatContext>
    auto format(const authpp::Message& message, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", authpp::util::byteDataToString(message.get(), message.size()));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};
