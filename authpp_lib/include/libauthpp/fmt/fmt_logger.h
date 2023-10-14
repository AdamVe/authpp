#pragma once

#include "fmt/format.h"

#include "../logger.h"

template <>
struct fmt::formatter<authpp::Logger::Level> {
    template <typename FormatContext>
    auto format(const authpp::Logger::Level& level, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", std::to_underlying(level));
    }
};
