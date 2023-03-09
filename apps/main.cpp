#include <libusb-1.0/libusb.h>

#include <algorithm>
#include <ranges>
#include <vector>

#include "ccid_connection.h"
#include "logger.h"
#include "oath_session.h"
#include "time_util.h"
#include "usb_manager.h"

using namespace authpp;

int main()
{
    Logger log("main");
    Logger::setLevel(Logger::Level::kDebug);

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    auto keys = usbManager.pollUsbDevices(matchVendorYubico);
    if (!keys.empty()) {
        UsbDevice::Connection usbConnection(keys[0]);
        CcidConnection conn(usbConnection);
        oath::Session oath_session(conn);

        auto credentials = oath_session.calculateAll(TimeUtil::getTimeStep());
#ifdef __cpp_lib_ranges
        std::ranges::sort(credentials, {}, &authpp::oath::Credential::name);
#else
        std::sort(credentials.begin(), credentials.end(),
            [](const authpp::oath::Credential& lhs,
                const authpp::oath::Credential& rhs) {
                return lhs.name < rhs.name;
            });
#endif
        for (auto&& c : credentials) {
            log.i("{} {}", c.name, c.code.value);
        }
    }

    return 0;
}
