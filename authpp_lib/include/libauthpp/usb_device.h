#pragma once

#include <libusb-1.0/libusb.h>

#include <string>
#include <vector>

namespace authpp {

class UsbDevice {
public:
    struct Interface {
        int number { -1 };
        int altSetting {};
        unsigned char endPointIn {};
        unsigned char endPointOut {};
        uint16_t maxPacketSizeIn {};
        uint16_t maxPacketSizeOut {};
    };

    class Connection {
    public:
        explicit Connection(const UsbDevice& usbDevice);
        ~Connection();

        libusb_device_handle* operator*() const;

        [[nodiscard]] Interface claimInterface(int usbClass, int usbSubclass) const;

    private:
        const UsbDevice& usbDevice;
        libusb_device_handle* handle;
    };
    explicit UsbDevice(libusb_device* device);
    ~UsbDevice();

    UsbDevice(const UsbDevice&);
    UsbDevice(UsbDevice&&) noexcept;

    UsbDevice& operator=(const UsbDevice&) = delete;
    UsbDevice& operator=(UsbDevice&&) = delete;

    std::string getManufacturer() const;
    std::string getProduct() const;
    std::string getSerialNumber() const;
    std::string toString() const;

private:
    std::string getStringDescriptor(std::size_t index) const;

    void openConnection(libusb_device_handle**) const;
    void closeConnection(libusb_device_handle**) const;

    libusb_device* device;

    libusb_device_descriptor deviceDescriptor;

    mutable std::string manufacturer;
    mutable bool readManufacturer { false };

    mutable std::string product;
    mutable bool readProduct { false };

    mutable std::string serialNumber;
    mutable bool readSerialNumber { false };
};

} // authpp
