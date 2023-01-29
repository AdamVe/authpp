#include "ByteArray.h"
#include "CcidConnection.h"
#include "Logger.h"
#include "Message.h"
#include "OathSession.h"
#include "UsbDevice.h"
#include "UsbDeviceHandle.h"
#include "Util.h"

#include <vector>

#include <libusb-1.0/libusb.h>

#define VENDOR_YUBICO (0x1050)

using namespace authpp;

int main()
{
    Logger log("main");
    Logger::setLevel(Logger::Level::verbose);

    libusb_init(nullptr);
    // libusb_set_option(nullptr, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

    auto* version { libusb_get_version() };

    log.i("Using libusb version: {}.{}.{}", version->major,
        version->minor, version->micro);

    log.d("hot-plug support: {}",
        libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG));

    // enumerate current USB devices
    libusb_device** devices;

    std::size_t connected_devices_count = libusb_get_device_list(NULL, &devices);

    std::vector<libusb_device*> yubiKeyUsbDevices;

    for (std::size_t&& index = 0; index < connected_devices_count; ++index) {
        auto* d = devices[index];
        // get descriptor
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(d, &desc);
        if (desc.idVendor == VENDOR_YUBICO) {
            yubiKeyUsbDevices.push_back(d);
        }
    }

    if (yubiKeyUsbDevices.size() == 0) {
        log.e("No connected YubiKey detected. Please connect a test YubiKey");
    } else {

        for (auto&& yubikey : yubiKeyUsbDevices) {
            UsbDevice yubiKey(yubikey);
            log.d("Yubikey: {}", yubiKey.toString());

            UsbConnection handle(yubikey);
            CcidConnection conn(handle);

            OathSession oathSession(conn);
            oathSession.list_credentials();
        }
    }

    libusb_free_device_list(devices, 1);
    libusb_exit(nullptr);

    return 0;
}
