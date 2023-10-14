#include <common/arg_parser.h>

#include <libauthpp/logger.h>
#include <libauthpp/oath_session_helper.h>
#include <libauthpp/time_util.h>
#include <libauthpp/usb_manager.h>

using namespace authpp;

int main(int argc, char** argv)
{
    common::ArgParser argParser(argc, argv);

    if (argParser.hasParam("-D")) {
        Logger::setLevel(Logger::Level::DEBUG);
    }

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    auto timeStep = TimeUtil::getTotpTimeStep(TimeUtil::getCurrentSeconds());

    auto keys = usbManager.pollUsbDevices(matchVendorYubico);
    if (!keys.empty()) {
        if (argParser.hasParam("-p")) {
            authpp::oath::pwd = argParser.getParamValue("-p");
        }

        if (argParser.hasParam("all")) {
            auto credentials = authpp::oath::calculateAll(keys[0], timeStep);
            for (auto&& c : credentials) {
                fmt::print("{} {}\n", c.name, c.code.value);
            }
        }

        if (argParser.hasParam("list")) {
            auto credentials = authpp::oath::listCredentials(keys[0]);
            for (auto&& c : credentials) {
                fmt::print("{}\n", c.name);
            }
        }

        if (argParser.hasParam("get")) {
            auto name = argParser.getParamValue("get");
            if (!name.empty()) {
                auto c = authpp::oath::calculate(keys[0], timeStep, name);
                fmt::print("{}\n", c.code.value);
            } else {
                fmt::print("get needs a value\n");
            }
        }
    }

    return 0;
}
