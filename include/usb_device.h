#pragma once

#include <libusb-1.0/libusb.h>

#include <string>
#include <vector>

namespace authpp {

class UsbDevice {
 public:
  struct Interface {
    int number{-1};
    int altsetting;
    unsigned char endpoint_in;
    unsigned char endpoint_out;
    uint16_t max_packet_size_in;
    uint16_t max_packet_size_out;
  };

  class Connection {
   public:
    explicit Connection(const UsbDevice& usb_device);
    ~Connection();

    libusb_device_handle* operator*() const;

    Interface claimInterface(int usbClass, int usbSubclass) const;

   private:
    const UsbDevice& usb_device;
    libusb_device_handle* handle;
  };
  explicit UsbDevice(libusb_device* device);
  ~UsbDevice();

  std::string GetManufacturer() const;
  std::string GetProduct() const;
  std::string ToString() const;

 private:
  std::string GetStringDescriptor(std::size_t index) const;

  void OpenConnection(libusb_device_handle**) const;
  void CloseConnection(libusb_device_handle**) const;

  libusb_device* device;

  libusb_device_descriptor device_descriptor;
  std::vector<libusb_config_descriptor*> config_descriptors;

  mutable std::string manufacturer;
  mutable bool read_manufacturer{false};

  mutable std::string product;
  mutable bool read_product{false};
};

}  // namespace authpp
