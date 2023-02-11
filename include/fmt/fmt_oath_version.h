#pragma once

#include <fmt/format.h>

#include "oath_session.h"

template <>
struct fmt::formatter<authpp::OathSession::Version> {
  template <typename FormatContext>
  auto format(const authpp::OathSession::Version& version,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}.{}.{}", version.major, version.minor,
                          version.patch);
  }

  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    return it;
  }
};
