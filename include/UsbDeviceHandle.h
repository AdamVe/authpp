#pragma once

#include <libusb-1.0/libusb.h>

namespace authpp {

class UsbDeviceHandle {
public:
    explicit UsbDeviceHandle(libusb_device* device);

    ~UsbDeviceHandle();

    libusb_device_handle* operator*() const;

private:
    libusb_device_handle* handle;
};

} // namespace authpp
