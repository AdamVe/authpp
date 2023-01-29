#include "UsbDeviceHandle.h"

#include <stdexcept>

namespace authpp {

UsbConnection::UsbConnection(libusb_device* device)
{
    if (0 != libusb_open(device, &handle)) {
        throw std::runtime_error("Failed to open device");
    }
}

UsbConnection::~UsbConnection()
{
    libusb_close(handle);
}

libusb_device_handle* UsbConnection::operator*() const
{
    return handle;
}

} // namespace authpp
