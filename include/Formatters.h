#pragma once

#include "Logger.h"

template <> struct fmt::formatter<authpp::Logger::Level> {
    template <typename FormatContext>
    auto format(const auth::Logger::Level &level, FormatContext &ctx) const
    -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", std::to_underlying(level));
    }
};
