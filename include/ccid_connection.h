#pragma once

#include "byte_array.h"
#include "usb_device.h"

namespace authpp {

class Message;

class CcidConnection {
public:
    explicit CcidConnection(const UsbDevice::Connection& handle);
    virtual ~CcidConnection();

    template <typename T>
    ByteArray Transcieve(T&&, int* transferred = nullptr) const;

private:
    void Setup();

    const UsbDevice::Connection& handle;
    UsbDevice::Interface usb_interface;

    std::byte slot { 1 };
    std::byte sequence { 1 };

    const int USB_CLASS_CSCID { 0x0b };
};

} // namespace authpp
