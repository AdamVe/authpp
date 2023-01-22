#pragma once

#include <Logger.h>

#include <libusb-1.0/libusb.h>

#include <string>
#include <vector>

namespace authpp {

class UsbDevice {
  public:
    explicit UsbDevice(libusb_device* device);
    ~UsbDevice();

    std::string getManufacturer() const;
    std::string getProduct() const;
    std::string toString() const;

  private:
    std::string getStringDescriptor(std::size_t index) const;

    libusb_device* device;

    libusb_device_descriptor device_descriptor;
    std::vector<libusb_config_descriptor*> config_descriptors;

    mutable std::string manufacturer;
    mutable bool read_manufacturer {false};

    mutable std::string product;
    mutable bool read_product {false};

    Logger log;
};

} // namespace authpp
