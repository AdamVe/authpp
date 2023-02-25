#pragma once

#include <fmt/format.h>

#include "oath_session.h"

template <>
struct fmt::formatter<authpp::Type> {
    template <typename FormatContext>
    auto format(const authpp::Type& type, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        switch (type) {
        case authpp::Type::HOTP:
            return fmt::format_to(ctx.out(), "HOTP");
        case authpp::Type::TOTP:
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
struct fmt::formatter<authpp::Algorithm> {
    template <typename FormatContext>
    auto format(const authpp::Algorithm& algorithm, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        switch (algorithm) {
        case authpp::Algorithm::HMAC_SHA1:
            return fmt::format_to(ctx.out(), "HMAC_SHA1");
        case authpp::Algorithm::HMAC_SHA256:
            return fmt::format_to(ctx.out(), "HMAC_SHA256");
        case authpp::Algorithm::HMAC_SHA512:
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
struct fmt::formatter<authpp::Credential> {
    template <typename FormatContext>
    auto format(const authpp::Credential& credential, FormatContext& ctx) const
        -> decltype(ctx.out())
    {
        return fmt::format_to(
            ctx.out(), "[{};{};{}]", credential.name, credential.type, credential.algorithm);
    }

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        return it;
    }
};

template <>
struct fmt::formatter<authpp::OathSession::Version> {
    template <typename FormatContext>
    auto format(const authpp::OathSession::Version& version,
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
