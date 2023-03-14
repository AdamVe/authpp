#include <algorithm>
#include <span>
#include <string>
#include <vector>

namespace authpp::common {
class ArgParser {
public:
    ArgParser(int argc, char** argv)
        : args(buildArgs(argc, argv))
    {
    }

    bool hasParam(std::string_view value) const
    {
        // return std::ranges::count(args, value) > 0;
        return std::find(args.cbegin(), args.cend(), std::string(value)) != args.cend();
    }

    std::string getParamValue(std::string_view value) const
    {
#if __cpp_ranges_lib > 22010101L
        auto i = std::ranges::find(params, value);
#else
        auto i = std::find(args.cbegin(), args.cend(), value);
#endif
        if (i++ != args.cend() && i != args.cend()) {
            return *i;
        }
        return {};
    }

private:
    using args_t = std::vector<std::string>;

    args_t buildArgs(int argc, char** argv)
    {
        args_t retval;
        std::span args_span { argv, argv + argc };
        for (auto&& arg : args_span) {
            retval.emplace_back(arg);
        }
        return retval;
    }

    const args_t args;
};
}
