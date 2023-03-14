#include <iostream>

#include <common/arg_parser.h>

#include "logger.h"
#include "oath_session_helper.h"
#include "usb_manager.h"

using namespace authpp;

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
            authpp::oath::pwd = argParser.getParamValue("-p");
        }

        if (argParser.hasParam("all")) {
            auto credentials = authpp::oath::calculateAll(keys[0]);
            for (auto&& c : credentials) {
                std::cout << c.name << " " << c.code.value << std::endl;
            }
        }

        if (argParser.hasParam("list")) {
            auto credentials = authpp::oath::listCredentials(keys[0]);
            for (auto&& c : credentials) {
                std::cout << c.name << " " << c.code.value << std::endl;
            }
        }

        if (argParser.hasParam("get")) {
            auto name = argParser.getParamValue("get");
            if (!name.empty()) {
                auto c = authpp::oath::calculate(keys[0], name);
                std::cout << c.code.value << std::endl;
            } else {
                std::cerr << "get needs a value" << std::endl;
            }
        }
    }

    return 0;
}
