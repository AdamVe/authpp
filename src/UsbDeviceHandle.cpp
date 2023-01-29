#include "UsbDeviceHandle.h"

#include <stdexcept>

namespace authpp {

UsbDeviceHandle::UsbDeviceHandle(libusb_device* device)
{
    if (0 != libusb_open(device, &handle)) {
        throw std::runtime_error("Failed to open device");
    }
}

UsbDeviceHandle::~UsbDeviceHandle()
{
    libusb_close(handle);
}

libusb_device_handle* UsbDeviceHandle::operator*() const
{
    return handle;
}

} // namespace authpp
