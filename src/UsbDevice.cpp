#include "UsbDevice.h"
#include "UsbDeviceHandle.h"

#include "Logger.h"

#include <libusb-1.0/libusb.h>

#include <string>

namespace authpp {

UsbDevice::UsbDevice(libusb_device* device)
    : device(libusb_ref_device(device))
{

    if (0 != libusb_get_device_descriptor(device, &device_descriptor)) {
        throw std::runtime_error("Failed to acquire device descriptor");
    };

    config_descriptors.resize(device_descriptor.bNumConfigurations);
    for (std::size_t configurationNum = 0; configurationNum < device_descriptor.bNumConfigurations; ++configurationNum) {
        if (0 != libusb_get_config_descriptor(device, configurationNum, &config_descriptors[configurationNum])) {
            throw std::runtime_error("Failed to acquire configuration descriptor");
        }
    }
}

UsbDevice::~UsbDevice()
{
    libusb_unref_device(device);
}

std::string UsbDevice::getManufacturer() const
{
    if (!read_manufacturer) {
        try {
            manufacturer = getStringDescriptor(device_descriptor.iManufacturer);
        } catch (const std::runtime_error& error) {
            Log.e("Failed to get manufacturer: {}", error.what());
        }

        read_manufacturer = true;
    }

    return manufacturer;
}

std::string UsbDevice::getProduct() const
{
    if (!read_product) {
        try {
            product = getStringDescriptor(device_descriptor.iProduct);
        } catch (const std::runtime_error& error) {
            Log.e("Failed to get product: {}", error.what());
        }
        read_product = true;
    }

    return product;
}

std::string UsbDevice::toString() const
{
    return fmt::format("device_descriptor[bLenght={},bDescriptorType={},bcdUSB={},"
                       "bDeviceClass={},bDeviceSubClass={},bDeviceProtocol={},bMaxPacketSize0={},"
                       "idVendor={:04x},idProduct={:04x},bcdDevice={},iManufacturer={}({}),iProduct={}({}),"
                       "iSerialNumber={},bNumConfigurations={}",
        device_descriptor.bLength, device_descriptor.bDescriptorType, device_descriptor.bcdUSB, device_descriptor.bDeviceClass, device_descriptor.bDeviceSubClass, device_descriptor.bDeviceProtocol, device_descriptor.bMaxPacketSize0, device_descriptor.idVendor, device_descriptor.idProduct, device_descriptor.bcdDevice, device_descriptor.iManufacturer, getManufacturer(), device_descriptor.iProduct, getProduct(), device_descriptor.iSerialNumber, device_descriptor.bNumConfigurations);
}

std::string UsbDevice::getStringDescriptor(std::size_t index) const
{

    UsbDeviceHandle deviceHandle(device);

    constexpr std::size_t length { 128 };
    unsigned char serial[length];

    if (auto err = libusb_get_string_descriptor_ascii(*deviceHandle, index, serial, length); err < 0) {
        throw std::runtime_error(fmt::format("Error getting string resource on index {}: {}({})",
            index, libusb_error_name(err), err));
    }
    return std::string((char*)serial);
}

} // namespace authpp
