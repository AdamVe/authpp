#pragma once

#include <libusb-1.0/libusb.h>

#include <functional>
#include <vector>

#include "usb_device.h"

namespace authpp {

class UsbManager {
public:
    UsbManager();
    ~UsbManager();
    std::vector<UsbDevice> pollUsbDevices(
        const std::function<bool(libusb_device_descriptor)>& p, long timeoutMs = 5000);

private:
    libusb_context* context;

    std::vector<UsbDevice> poll(std::function<bool(libusb_device_descriptor)>);
};

} // authpp
