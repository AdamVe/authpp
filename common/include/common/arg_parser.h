#include <span>
#include <string>
#include <vector>

namespace authpp::common {
class ArgParser {
public:
    ArgParser(int argc, char** argv);
    bool hasParam(std::string_view value) const;
    std::string getParamValue(std::string_view value) const;

private:
    using args_t = std::vector<std::string>;
    args_t buildArgs(int argc, char** argv) const;
    const args_t args;
};
} // namespace authpp::common
