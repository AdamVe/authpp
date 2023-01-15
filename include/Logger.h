#pragma once

#include <iostream>
#include <utility>
#include <map>


#include <fmt/core.h>
#include <fmt/format.h>

class Logger {
  public:

    enum class Level : unsigned {
        info = 0,
        warning = 1,
        error = 2,
        debug = 3,
        verbose = 4
    };

    Logger(std::string&& name) : name(name) {
    }

    static void setLevel(Level level) {
        Logger::level = level;
    }

    template <typename T>
    void i(T && message) {
        log(Level::info, std::forward<T>(message));
    }

    template <typename T>
    void w(T && message) {
        log(Level::warning, std::forward<T>(message));
    }

    template <typename T>
    void e(T && message) {
        log(Level::error, std::forward<T>(message));
    }

    template <typename T>
    void d(T && message) {
        log(Level::debug, std::forward<T>(message));
    }

    template <typename T>
    void v(T && message) {
        log(Level::verbose, std::forward<T>(message));
    }

  private:
    static Level level;
    std::string name;

	std::map<Logger::Level, const char*> enumNames {
    	{Logger::Level::info, "I"},
    	{Logger::Level::warning, "W"},
    	{Logger::Level::error, "E"},
    	{Logger::Level::debug, "D"},
    	{Logger::Level::verbose, "V"},
	};

    template <typename T>
    void log(Level messageLevel, T && message) {
        if (messageLevel <= Logger::level) {
            fmt::print("[{}] {} {}\n", enumNames[messageLevel], name, message);
        }
    }
};


template <> struct fmt::formatter<Logger::Level> {
    char presentation = 'f';

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const Logger::Level& level, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", std::to_underlying(level));
    }
};


