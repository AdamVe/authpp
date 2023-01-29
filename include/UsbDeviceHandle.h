#pragma once

#include <libusb-1.0/libusb.h>

namespace authpp {

class UsbConnection {
public:
    explicit UsbConnection(libusb_device* device);

    ~UsbConnection();

    libusb_device_handle* operator*() const;

private:
    libusb_device_handle* handle;
};

} // namespace authpp
