#pragma once

#include <string>
#include <utility>
#include <vector>

#include <fmt/core.h>

namespace authpp {

class Logger {
public:
    enum class Level : unsigned {
        info = 0,
        warning = 1,
        error = 2,
        debug = 3,
        verbose = 4
    };

    explicit Logger(std::string_view name = "")
        : name(name)
    {
    }

    static void setLevel(Level level)
    {
        Logger::level = level;
    }

    template <typename... Args>
    void i(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::info, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void w(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::warning, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void e(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::error, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void v(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::verbose, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void d(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::debug, fmt, std::forward<Args>(args)...);
    }

private:
    inline static Level level { Level::error };
    std::string name;

    inline static std::vector<std::string_view> enumNames { "I", "W", "E", "D", "V" };

    template <typename... Args>
    void log(Level level, fmt::format_string<Args...> fmt, Args&&... args) const
    {
        if (level <= Logger::level) {
            if (name.length() > 0) {
                fmt::print("[{}] <{}> {}\n",
                    enumNames[std::to_underlying(level)],
                    name,
                    fmt::vformat(fmt, fmt::make_format_args(args...)));
            } else {
                fmt::print("[{}] {}\n",
                    enumNames[std::to_underlying(level)],
                    fmt::vformat(fmt, fmt::make_format_args(args...)));
            }
        }
    }
};

} // namespace authpp
