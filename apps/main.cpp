#include <libusb-1.0/libusb.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "ccid_connection.h"
#include "logger.h"
#include "message.h"
#include "oath_session.h"
#include "usb_device.h"
#include "util.h"

#define VENDOR_YUBICO (0x1050)

using namespace authpp;

long getTimeStep()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    long timeStep = seconds / 30;
    return timeStep;
}

int main()
{
    Logger log("main");
    Logger::setLevel(Logger::Level::kDebug);

    libusb_init(nullptr);

    auto* version { libusb_get_version() };

    log.i("Using libusb version: {}.{}.{}", version->major, version->minor, version->micro);

    libusb_device** devices;

    std::size_t connected_devices_count = libusb_get_device_list(NULL, &devices);

    for (std::size_t&& index = 0; index < connected_devices_count; ++index) {
        auto* d = devices[index];
        // get descriptor
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(d, &desc);
        if (desc.idVendor == VENDOR_YUBICO) {
            UsbDevice yubiKey(d);
            log.d("Yubikey: {}", yubiKey.ToString());

            UsbDevice::Connection usbConnection(yubiKey);
            CcidConnection conn(usbConnection);
            oath::Session oath_session(conn);

            auto credentials = oath_session.calculateAll(getTimeStep());
            std::ranges::sort(credentials, {}, &authpp::oath::Credential::name);
            for (auto&& c : credentials) {
                log.i("{} {}", c.name, c.code.value);
            }
        }
    }

    libusb_free_device_list(devices, 1);
    libusb_exit(nullptr);

    return 0;
}
