#pragma once

#include "fmt/core.h"

#include <string>
#include <utility>
#include <vector>

namespace authpp {

class Logger {
public:
    enum class Level : unsigned { INFO = 0, WARNING = 1, ERROR = 2, DEBUG = 3, VERBOSE = 4 };

    explicit Logger(std::string_view name = "")
        : name(name)
    {
    }

    static void setLevel(Level loggerLevel) { Logger::level = loggerLevel; }

    template <typename... Args>
    void i(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::INFO, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void w(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::WARNING, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void e(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::ERROR, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void v(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::VERBOSE, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void d(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::DEBUG, fmt, std::forward<Args>(args)...);
    }

private:
    inline static Level level { Level::ERROR };
    std::string name;

    inline static std::vector<std::string_view> enumNames { "I", "W", "E", "D", "V" };

    template <typename... Args>
    void log(Level loggerLevel, fmt::format_string<Args...> fmt, Args&&... args) const
    {
        if (loggerLevel <= Logger::level) {
            if (name.length() > 0) {
                fmt::print("[{}] <{}> {}\n", enumNames[std::to_underlying(loggerLevel)], name,
                    fmt::vformat(fmt, fmt::make_format_args(args...)));
            } else {
                fmt::print("[{}] {}\n", enumNames[std::to_underlying(loggerLevel)],
                    fmt::vformat(fmt, fmt::make_format_args(args...)));
            }
        }
    }
};

} // authpp
