#include "oath_session_helper.h"

#include <string>
#include <vector>
#include <algorithm>

namespace authpp::oath {

std::vector<Credential> calculateAll(const UsbDevice& key)
{
    return useOathSession<std::vector<Credential>>(key, [](auto& session) {
        auto credentials = session.calculateAll(TimeUtil::getTimeStep());
#if __cpp_lib_ranges >= 202106L
        std::ranges::sort(credentials, oath::Credential::compareByName);
#else
        std::sort(credentials.begin(), credentials.end(), oath::Credential::compareByName);
#endif
        return credentials;
    });
}

std::vector<Credential> listCredentials(const UsbDevice& key)
{
    return useOathSession<std::vector<Credential>>(key, [](auto& session) {
        auto credentials = session.listCredentials();
#if __cpp_lib_ranges >= 202106L
        std::ranges::sort(credentials, oath::Credential::compareByName);
#else
        std::sort(credentials.begin(), credentials.end(), oath::Credential::compareByName);
#endif
        return credentials;
    });
}

Credential calculate(const UsbDevice& key, std::string_view name)
{
    return useOathSession<Credential>(key, [&name](auto& session) {
        auto c = session.calculateOne(TimeUtil::getTimeStep(), name);
        return c;
    });
}
} // namespace authpp::oath
