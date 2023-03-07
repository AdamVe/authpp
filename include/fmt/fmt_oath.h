#pragma once

#include <fmt/format.h>

#include "oath_session.h"

template <>
struct fmt::formatter<authpp::oath::Type> {
    template <typename FormatContext>
    auto format(const authpp::oath::Type& type, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        switch (type) {
        case authpp::oath::Type::HOTP:
            return fmt::format_to(ctx.out(), "HOTP");
        case authpp::oath::Type::TOTP:
            return fmt::format_to(ctx.out(), "TOTP");
        }
        return fmt::format_to(ctx.out(), "HOTP");
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};

template <>
struct fmt::formatter<authpp::oath::Algorithm> {
    template <typename FormatContext>
    auto format(const authpp::oath::Algorithm& algorithm, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        switch (algorithm) {
        case authpp::oath::Algorithm::HMAC_SHA1:
            return fmt::format_to(ctx.out(), "HMAC_SHA1");
        case authpp::oath::Algorithm::HMAC_SHA256:
            return fmt::format_to(ctx.out(), "HMAC_SHA256");
        case authpp::oath::Algorithm::HMAC_SHA512:
            return fmt::format_to(ctx.out(), "HMAC_SHA512");
        }
        return fmt::format_to(ctx.out(), "HMAC_SHA1");
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};

template <>
struct fmt::formatter<authpp::oath::Credential> {
    template <typename FormatContext>
    auto format(const authpp::oath::Credential& credential, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(
            ctx.out(), "[{};{};{}]", credential.name, credential.code.type, credential.algorithm);
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};

template <>
struct fmt::formatter<authpp::oath::Version> {
    template <typename FormatContext>
    auto format(const authpp::oath::Version& version,
        FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}.{}.{}", version.major, version.minor,
            version.patch);
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};
