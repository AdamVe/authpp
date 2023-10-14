#include "usb_device.h"

#include <libusb-1.0/libusb.h>

#include <string>

#include "logger.h"

namespace authpp {

namespace {
    Logger log("UsbDevice");
}

UsbDevice::UsbDevice(libusb_device* device)
    : device(libusb_ref_device(device))
    , deviceDescriptor()
{
    if (0 != libusb_get_device_descriptor(device, &deviceDescriptor)) {
        throw std::runtime_error("Failed to acquire device descriptor");
    }
}

UsbDevice::~UsbDevice()
{
    libusb_unref_device(device);
}

UsbDevice::UsbDevice(UsbDevice&& other) noexcept
    : device(other.device)
    , deviceDescriptor()
{
    if (0 != libusb_get_device_descriptor(device, &deviceDescriptor)) {
        throw std::runtime_error("Failed to acquire device descriptor");
    }
    other.device = nullptr;
}

UsbDevice::UsbDevice(const UsbDevice& other)
    : device(libusb_ref_device(other.device))
    , deviceDescriptor()
{
    if (0 != libusb_get_device_descriptor(device, &deviceDescriptor)) {
        throw std::runtime_error("Failed to acquire device descriptor");
    }
}

std::string UsbDevice::getManufacturer() const
{
    if (!readManufacturer) {
        try {
            manufacturer = getStringDescriptor(deviceDescriptor.iManufacturer);
        } catch (const std::runtime_error& error) {
            log.e("Failed to get manufacturer: {}", error.what());
        }

        readManufacturer = true;
    }

    return manufacturer;
}

std::string UsbDevice::getProduct() const
{
    if (!readProduct) {
        try {
            product = getStringDescriptor(deviceDescriptor.iProduct);
        } catch (const std::runtime_error& error) {
            log.e("Failed to get product: {}", error.what());
        }
        readProduct = true;
    }

    return product;
}

std::string UsbDevice::getSerialNumber() const
{
    if (!readSerialNumber) {
        Connection deviceHandle(*this);
        try {
            serialNumber = getStringDescriptor(deviceDescriptor.iSerialNumber);
        } catch (const std::runtime_error& error) {
            log.e("Failed to get serial number: {}", error.what());
        }
        readSerialNumber = true;
    }

    return serialNumber;
}

std::string UsbDevice::toString() const
{
    return fmt::format(
        "device_descriptor[bLenght={},bDescriptorType={},bcdUSB={},"
        "bDeviceClass={},bDeviceSubClass={},bDeviceProtocol={},bMaxPacketSize0={"
        "}"
        ","
        "idVendor={:04x},idProduct={:04x},bcdDevice={},iManufacturer={}({}),"
        "iProduct={}({}),"
        "iSerialNumber={}({}),bNumConfigurations={}",
        deviceDescriptor.bLength, deviceDescriptor.bDescriptorType, deviceDescriptor.bcdUSB,
        deviceDescriptor.bDeviceClass, deviceDescriptor.bDeviceSubClass,
        deviceDescriptor.bDeviceProtocol, deviceDescriptor.bMaxPacketSize0,
        deviceDescriptor.idVendor, deviceDescriptor.idProduct, deviceDescriptor.bcdDevice,
        deviceDescriptor.iManufacturer, getManufacturer(), deviceDescriptor.iProduct, getProduct(),
        deviceDescriptor.iSerialNumber, getSerialNumber(), deviceDescriptor.bNumConfigurations);
}

std::string UsbDevice::getStringDescriptor(std::size_t index) const
{
    if (index == 0) {
        return {};
    }

    Connection deviceHandle(*this);

    constexpr std::size_t length { 128 };
    unsigned char stringDescriptor[length];

    if (auto err
        = libusb_get_string_descriptor_ascii(*deviceHandle, index, stringDescriptor, length);
        err < 0) {
        throw std::runtime_error(
            fmt::format("Error getting string resource on index {}: {}({})", index,
                libusb_error_name(err), err));
    }

    log.v("Read string descriptor from index {} with content {}", index,
        (const char*)stringDescriptor);

    return { ((char*)stringDescriptor) };
}

void UsbDevice::openConnection(libusb_device_handle** handle) const
{
    if (0 != libusb_open(device, handle)) {
        throw std::runtime_error("Failed to open device");
    }
}

void UsbDevice::closeConnection(libusb_device_handle** handle) const // NOLINT(*-convert-member-functions-to-static)
{
    libusb_close(*handle);
}

UsbDevice::Connection::Connection(const UsbDevice& usbDevice)
    : usbDevice(usbDevice)
    , handle()
{
    usbDevice.openConnection(&handle);
}

UsbDevice::Connection::~Connection() { usbDevice.closeConnection(&handle); }

libusb_device_handle* UsbDevice::Connection::operator*() const
{
    return handle;
}

UsbDevice::Interface UsbDevice::Connection::claimInterface(
    int usbClass, int usbSubClass) const
{
    log.v("Searching config for {:02x}:{:02x}", usbClass, usbSubClass);
    for (uint8_t configIndex = 0; configIndex < usbDevice.deviceDescriptor.bNumConfigurations;
         ++configIndex) {
        libusb_config_descriptor* config;
        if (0 != libusb_get_config_descriptor(usbDevice.device, configIndex, &config)) {
            libusb_free_config_descriptor(config);
            continue;
        }

        log.v("  Config has {} interfaces", config->bNumInterfaces);
        for (int interfaceNum = 0; interfaceNum < config->bNumInterfaces;
             ++interfaceNum) {
            log.v("  Interface {} of {}", interfaceNum, config->bNumInterfaces);
            auto usbInterface { config->interface[interfaceNum] };
            for (int altsettingNum = 0; altsettingNum < usbInterface.num_altsetting;
                 ++altsettingNum) {
                log.v("    altsetting {} of {}", altsettingNum,
                    usbInterface.num_altsetting);
                auto altsetting { usbInterface.altsetting[altsettingNum] };
                auto iClass = altsetting.bInterfaceClass;
                auto iSubClass = altsetting.bInterfaceSubClass;
                if (iClass == usbClass && iSubClass == usbSubClass) {
                    unsigned char ein = 0;
                    unsigned char eout = 0;
                    uint16_t maxIn = 0;
                    uint16_t maxOut = 0;

                    for (int endpointNum = 0; endpointNum < altsetting.bNumEndpoints;
                         ++endpointNum) {
                        auto endpoint = altsetting.endpoint[endpointNum];
                        log.v(
                            "      Endpoint {} bmAttributtes: {:02x} "
                            "bEndpointAddress: "
                            "{:02x}",
                            endpointNum, endpoint.bmAttributes, endpoint.bEndpointAddress);
                        if ((endpoint.bmAttributes & 0x03) == libusb_endpoint_transfer_type::LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK) {
                            log.v(
                                "        searching endpoints (addr={}, "
                                "dir={}, eaddr={})",
                                endpoint.bEndpointAddress, endpoint.bEndpointAddress & 0x80,
                                endpoint.bEndpointAddress & 0x0f);
                            if ((endpoint.bEndpointAddress & 0x80) == libusb_endpoint_direction::LIBUSB_ENDPOINT_OUT) {
                                log.v("          found LIBUSB_ENDPOINT_OUT");
                                eout = endpoint.bEndpointAddress;
                                maxOut = endpoint.wMaxPacketSize;
                            } else {
                                log.v("          found LIBUSB_ENDPOINT_IN");
                                ein = endpoint.bEndpointAddress;
                                maxIn = endpoint.wMaxPacketSize;
                            }
                        }
                    }

                    log.v(
                        "Found {:02x}:{:02x} with on interface {} with "
                        "altsetting {} "
                        "and endpoints IN={:02x}({}) OUT={:02x}({})",
                        iClass, iSubClass, interfaceNum, altsettingNum, ein, maxIn, eout, maxOut);
                    libusb_free_config_descriptor(config);
                    return UsbDevice::Interface {
                        interfaceNum, altsettingNum, (unsigned char)ein,
                        (unsigned char)eout, maxIn, maxOut };
                }
            }
        }
    }

    log.v("Failed to find required interface with endpoints for {:02x}:{:02x}",
        usbClass, usbSubClass);
    return {};
}

} // authpp
