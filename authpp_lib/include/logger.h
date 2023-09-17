#pragma once

#include <fmt/core.h>

#include <string>
#include <utility>
#include <vector>

namespace authpp {

class Logger {
public:
    enum class Level : unsigned {
        kInfo = 0,
        kWarning = 1,
        kError = 2,
        kDebug = 3,
        kVerbose = 4
    };

    explicit Logger(std::string_view name = "")
        : name(name)
    {
    }

    static void setLevel(Level loggerLevel) { Logger::level = loggerLevel; }

    template <typename... Args>
    void i(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::kInfo, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void w(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::kWarning, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void e(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::kError, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void v(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::kVerbose, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void d(fmt::format_string<Args...> fmt, Args&&... args) const
    {
        log(Level::kDebug, fmt, std::forward<Args>(args)...);
    }

private:
    inline static Level level { Level::kError };
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

} // namespace authpp
