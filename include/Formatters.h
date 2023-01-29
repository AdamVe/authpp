#pragma once

#include "Logger.h"
#include "Message.h"

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
    template <typename FormatContext>
    auto format(const authpp::ByteArray& byteArray, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", authpp::util::byteDataToString(byteArray.get(), byteArray.getDataSize()));
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
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
