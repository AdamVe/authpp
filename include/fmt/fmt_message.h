#pragma once

#include <fmt/format.h>

#include "message.h"
#include "util.h"

template <>
struct fmt::formatter<authpp::Message> {
  template <typename FormatContext>
  auto format(const authpp::Message& message, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(
        ctx.out(), "{}",
        authpp::util::ByteDataToString(message.Get(), message.Size()));
  }

  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    return it;
  }
};
