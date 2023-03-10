#include <algorithm>
#include <iostream>
#include <ranges>
#include <string_view>
#include <vector>

#include "ccid_connection.h"
#include "logger.h"
#include "oath_session.h"
#include "time_util.h"
#include "usb_manager.h"

using namespace authpp;

void useOathSession(const UsbDevice& key, std::function<void(const oath::Session&)> f)
{
    UsbDevice::Connection usbConnection(key);
    CcidConnection conn(usbConnection);
    oath::Session oath_session(conn);
    f(oath_session);
}

void listCredentials(const UsbDevice& key)
{
    useOathSession(key, [](auto& session) {
        auto credentials = session.calculateAll(TimeUtil::getTimeStep());
#ifdef __cpp_lib_ranges
        std::ranges::sort(credentials, oath::Credential::compareByName);
#else
        std::sort(credentials.begin(), credentials.end(), oath::Credential::compareByName);
#endif
        for (auto&& c : credentials) {
            std::cout << c.name << " " << c.code.value << std::endl;
        }
    });
}

void getCode(const UsbDevice& key, std::string_view name)
{
    useOathSession(key, [&name](auto& session) {
        auto c = session.calculateOne(TimeUtil::getTimeStep(), name);
        std::cout << c.code.value << std::endl;
    });
}

bool hasParam(const std::span<char*>& params, std::string_view value)
{
    return std::ranges::count(params, value) > 0;
}

std::string getParamValue(const std::span<char*>& params, std::string_view value)
{
    auto i = std::ranges::find(params, value);
    if (i++ != params.end() && i != params.end()) {
        return *i;
    }
    return {};
}

int main(int argc, char** argv)
{
    Logger log("main");

    std::span params { argv, argv + argc };

    if (hasParam(params, "-D")) {
        Logger::setLevel(Logger::Level::kDebug);
    }

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    auto keys = usbManager.pollUsbDevices(matchVendorYubico);
    if (!keys.empty()) {
        if (hasParam(params, "list")) {
            listCredentials(keys[0]);
        }

        if (hasParam(params, "get")) {
            auto name = getParamValue(params, "get");
            if (!name.empty()) {
                getCode(keys[0], name);
            } else {
                std::cerr << "get needs a value" << std::endl;
            }
        }
    }

    return 0;
}
