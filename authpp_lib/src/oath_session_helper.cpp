#include "oath_session_helper.h"
#include "time_util.h"

#include <algorithm>
#include <string>
#include <vector>

namespace authpp::oath {

std::vector<Credential> calculateAll(const UsbDevice& key, long secondsSinceEpoch)
{
    auto timeStep = TimeUtil::getTotpTimeStep(secondsSinceEpoch);
    return useOathSession<std::vector<Credential>>(key, [timeStep](Session& session) {
        auto credentials = session.calculateAll(timeStep);
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

Credential calculate(const UsbDevice& key, long secondsSinceEpoch, std::string_view name)
{
    auto timeStep = TimeUtil::getTotpTimeStep(secondsSinceEpoch);
    return useOathSession<Credential>(key, [&name, timeStep](Session& session) {
        auto c = session.calculateOne(timeStep, name);
        return c;
    });
}
} // authpp::oath
