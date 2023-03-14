#include <algorithm>
#include <iostream>
#include <ranges>
#include <string_view>
#include <vector>

#include <common/arg_parser.h>

#include "ccid_connection.h"
#include "logger.h"
#include "oath_session.h"
#include "time_util.h"
#include "usb_manager.h"

using namespace authpp;

namespace {
std::string pwd;
}

void useOathSession(const UsbDevice& key, std::function<void(oath::Session&)> f)
{
    UsbDevice::Connection usbConnection(key);
    CcidConnection conn(usbConnection);
    oath::Session oath_session(conn);
    if (!pwd.empty()) {
        oath_session.unlock(pwd);
    }
    f(oath_session);
}

void listCredentials(const UsbDevice& key)
{
    useOathSession(key, [](auto& session) {
        auto credentials = session.listCredentials();
#if __cpp_lib_ranges > 202110L
        std::ranges::sort(credentials, oath::Credential::compareByName);
#else
        std::sort(credentials.begin(), credentials.end(), oath::Credential::compareByName);
#endif
        for (auto&& c : credentials) {
            std::cout << c.name << " " << c.code.value << std::endl;
        }
    });
}

void calculateAll(const UsbDevice& key)
{
    useOathSession(key, [](auto& session) {
        auto credentials = session.calculateAll(TimeUtil::getTimeStep());
#if __cpp_lib_ranges > 202110L
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

int main(int argc, char** argv)
{
    common::ArgParser argParser(argc, argv);

    if (argParser.hasParam("-D")) {
        Logger::setLevel(Logger::Level::kDebug);
    }

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    auto keys = usbManager.pollUsbDevices(matchVendorYubico);
    if (!keys.empty()) {
        if (argParser.hasParam("-p")) {
            pwd = argParser.getParamValue("-p");
        }

        if (argParser.hasParam("all")) {
            calculateAll(keys[0]);
        }

        if (argParser.hasParam("list")) {
            listCredentials(keys[0]);
        }

        if (argParser.hasParam("get")) {
            auto name = argParser.getParamValue("get");
            if (!name.empty()) {
                getCode(keys[0], name);
            } else {
                std::cerr << "get needs a value" << std::endl;
            }
        }
    }

    return 0;
}
