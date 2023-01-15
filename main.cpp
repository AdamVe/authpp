#include <iostream>


#include <Logger.h>

#include <libusb-1.0/libusb.h>
#include <fmt/core.h>

#define VENDOR_YUBICO (0x1050)

class UsbDevice {
  public:
    UsbDevice(libusb_device* dev)
    : log("UsbDevice") {
        if (0 == libusb_open(dev, &handle)) {

			log.v(fmt::format("Acquired USB device handle"));
        };
    }

    virtual ~UsbDevice() {
        libusb_close(handle);
        log.v(fmt::format("Closed USB device handle"));
    }

  private:
  	Logger log;
    struct libusb_device_handle * handle;
};

int main() {
    Logger log("main");
    Logger::setLevel(Logger::Level::debug);

    libusb_init(NULL);

    auto* version { libusb_get_version() };

    log.i(fmt::format("Using libusb version: {}.{}.{}", version->major, version->minor, version->micro));

    log.d(fmt::format("hot-plug support: {}", libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)));

    // enumerate current usb devices
    libusb_device** devices;

    std::size_t connected_devices_count = libusb_get_device_list(NULL, &devices);
    std::size_t yubicoDevCount = 0;

    for(std::size_t&& index=0; index < connected_devices_count; ++index) {
        auto* d = devices[index];
        // get descriptor
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(d, &desc);
        if (desc.idVendor == VENDOR_YUBICO) {
            yubicoDevCount++;
            log.d(fmt::format(" vendor_id: {:04x} product_id: {:04x}", desc.idVendor, desc.idProduct));
        }

    }

    log.d(fmt::format("Yubikey count: {}", yubicoDevCount));

    libusb_free_device_list(devices, 0);
    libusb_exit(NULL);

    return 0;
}
