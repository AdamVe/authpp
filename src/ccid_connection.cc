#include "ccid_connection.h"

#include <libusb-1.0/libusb.h>

#include "byte_array.h"
#include "fmt/fmt_byte_array.h"
#include "fmt/fmt_message.h"
#include "logger.h"
#include "message.h"
#include "usb_device.h"
#include "util.h"

namespace authpp {

#define TIMEOUT 10000

namespace {
Logger log("CcidConnection");
}

CcidConnection::CcidConnection(const UsbDevice::Connection& handle)
    : handle(handle), usb_interface() {
  log.v("CCID connection opened");
  Setup();
  auto atr{Transcieve(Message((std::byte)0x62, ByteArray(0)))};
  log.v("ATR: {}", atr);
}

CcidConnection::~CcidConnection() { log.v("CCID connection closed"); }

template <typename T>
ByteArray CcidConnection::Transcieve(T&& message, int* transferred) const {
  int really_written = 0;
  if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_out,
                                     (unsigned char*)message.Get(),
                                     message.Size(), &really_written, TIMEOUT);
      err != 0) {
    throw new std::runtime_error(
        fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
  };
  log.v("send {}", message);

  // receive
  int really_recieved = 0;
  std::size_t array_len = usb_interface.max_packet_size_in;
  ByteArray byte_array(array_len);
  if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
                                     (unsigned char*)byte_array.Get(),
                                     array_len, &really_recieved, TIMEOUT);
      err != 0) {
    throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
                                             libusb_error_name(err), err));
  };

  byte_array.SetDataSize(really_recieved);
  log.v("recv {}", byte_array);

  // size of data
  int expected_data_size;
  expected_data_size =
      (unsigned)byte_array.Get()[1] | (unsigned)byte_array.Get()[2] << 8 |
      (unsigned)byte_array.Get()[3] << 16 | (unsigned)byte_array.Get()[4] << 24;

  ByteArray response_buffer(expected_data_size);
  int currentLength = really_recieved - 10;

  memcpy(response_buffer.Get(), byte_array.Get() + 10, currentLength);
  response_buffer.SetDataSize(really_recieved - 10);

  while (currentLength < expected_data_size) {
    // receive again
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
                                       (unsigned char*)byte_array.Get(),
                                       array_len, &really_recieved, TIMEOUT);
        err != 0) {
      throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
                                               libusb_error_name(err), err));
    }

    byte_array.SetDataSize(really_recieved);
    memcpy(response_buffer.Get() + currentLength, byte_array.Get(),
           really_recieved);
    currentLength += really_recieved;
    response_buffer.SetDataSize(currentLength);
  }

  if (transferred != nullptr) {
    *transferred = really_recieved;
  }

  return response_buffer;
}

void CcidConnection::Setup() {
  usb_interface = handle.claimInterface(USB_CLASS_CSCID, 0);
  if (usb_interface.number == -1) {
    throw new std::runtime_error("Failure finding correct CCID interface");
  }

  if (libusb_kernel_driver_active(*handle, usb_interface.number) == 1) {
    if (libusb_detach_kernel_driver(*handle, usb_interface.number) == 0) {
      log.v("detached kernel driver");
    }
  }

  if (libusb_claim_interface(*handle, usb_interface.number) != 0) {
    throw new std::runtime_error("Failure claiming CCID interface");
  }
}

}  // namespace authpp
