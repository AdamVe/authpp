#include "usb_device.h"

#include "logger.h"

#include <string>

#include <libusb-1.0/libusb.h>

namespace authpp {

namespace {
    Logger log("UsbDevice");
}

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
            log.e("Failed to get manufacturer: {}", error.what());
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
            log.e("Failed to get product: {}", error.what());
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

    Connection deviceHandle(*this);

    constexpr std::size_t length { 128 };
    unsigned char serial[length];

    if (auto err = libusb_get_string_descriptor_ascii(*deviceHandle, index, serial, length); err < 0) {
        throw std::runtime_error(fmt::format("Error getting string resource on index {}: {}({})",
            index, libusb_error_name(err), err));
    }
    return std::string((char*)serial);
}

void UsbDevice::openConnection(libusb_device_handle** handle) const
{
    if (0 != libusb_open(device, handle)) {
        throw std::runtime_error("Failed to open device");
    }
}

void UsbDevice::closeConnection(libusb_device_handle** handle) const
{
    libusb_close(*handle);
}

UsbDevice::Connection::Connection(const UsbDevice& usbDevice)
    : usbDevice(usbDevice)
{
    usbDevice.openConnection(&handle);
}

UsbDevice::Connection::~Connection()
{
    usbDevice.closeConnection(&handle);
}

libusb_device_handle* UsbDevice::Connection::operator*() const
{
    return handle;
}

UsbDevice::Interface UsbDevice::Connection::claimInterface(int usbClass, int usbSubClass) const
{
    log.v("Searching config for {:02x}:{:02x}", usbClass, usbSubClass);
    for (auto&& config : usbDevice.config_descriptors) {
        log.v("  Config has {} interfaces", config->bNumInterfaces);
        for (int interfaceNum = 0; interfaceNum < config->bNumInterfaces; ++interfaceNum) {
            log.v("  Interface {} of {}", interfaceNum, config->bNumInterfaces);
            auto usbInterface { config->interface[interfaceNum] };
            for (int altsettingNum = 0; altsettingNum < usbInterface.num_altsetting; ++altsettingNum) {
                log.v("    altsetting {} of {}", altsettingNum, usbInterface.num_altsetting);
                auto altsetting { usbInterface.altsetting[altsettingNum] };
                auto iClass = altsetting.bInterfaceClass;
                auto iSubClass = altsetting.bInterfaceSubClass;
                if (iClass == usbClass && iSubClass == usbSubClass) {
                    unsigned char ein = 0;
                    unsigned char eout = 0;
                    uint16_t max_in = 0;
                    uint16_t max_out = 0;

                    for (int endpointNum = 0; endpointNum < altsetting.bNumEndpoints; ++endpointNum) {
                        auto endpoint = altsetting.endpoint[endpointNum];
                        log.v("      Endpoint {} bmAttributtes: {:02x} bEndpointAddress: {:02x}", endpointNum, endpoint.bmAttributes, endpoint.bEndpointAddress);
                        if ((endpoint.bmAttributes & 0x03) == libusb_endpoint_transfer_type::LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK) {
                            log.v("        searching endpoints (addr={}, dir={}, eaddr={})", endpoint.bEndpointAddress, endpoint.bEndpointAddress & 0x80, endpoint.bEndpointAddress & 0x0f);
                            if ((endpoint.bEndpointAddress & 0x80) == libusb_endpoint_direction::LIBUSB_ENDPOINT_OUT) {
                                log.v("          found LIBUSB_ENDPOINT_OUT");
                                eout = endpoint.bEndpointAddress;
                                max_out = endpoint.wMaxPacketSize;
                            } else {
                                log.v("          found LIBUSB_ENDPOINT_IN");
                                ein = endpoint.bEndpointAddress;
                                max_in = endpoint.wMaxPacketSize;
                            }
                        }
                    }

                    if (ein > -1 && eout > -1) {
                        log.v("Found {:02x}:{:02x} with on interface {} with altsetting {} and endpoints IN={:02x}({}) OUT={:02x}({})", iClass, iSubClass, interfaceNum, altsettingNum, ein, max_in, eout, max_out);
                        return UsbDevice::Interface { interfaceNum, altsettingNum, (unsigned char)ein, (unsigned char)eout, max_in, max_out };
                    }
                }
            }
        }
    }

    log.v("Failed to find required interface with endpoints for {:02x}:{:02x}", usbClass, usbSubClass);
    return {};
}

} // namespace authpp
