#include "ccid_connection.h"

#include <libusb-1.0/libusb.h>

#include "logger.h"
#include "message.h"
#include "usb_device.h"
#include "util.h"

#include "fmt/fmt_bytes.h"
#include "fmt/fmt_message.h"

namespace authpp {

#define TIMEOUT 100

namespace {
    Logger log("CcidConnection");
}

CcidConnection::CcidConnection(const UsbDevice::Connection& handle)
    : handle(handle)
    , usb_interface()
{
    log.v("CCID connection opened");
    Setup();
    auto atr { Transcieve(Message(0x62, Bytes(0))) };
    log.v("ATR: {}", atr);
}

CcidConnection::~CcidConnection() { log.v("CCID connection closed"); }

Bytes CcidConnection::Transcieve(const Message& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_out,
            message.get().getRaw(),
            message.get().size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(
            fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message);

    // receive
    int really_recieved = 0;
    std::size_t array_len = usb_interface.max_packet_size_in;
    Bytes bytes(array_len);
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
            bytes.getRaw(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
            libusb_error_name(err), err));
    };

    bytes.setSize(really_recieved);
    log.v("recv {}", bytes);

    // size of data
    uint32_t expected_data_size;
    bytes.pointTo(1);
    bytes.getI32(expected_data_size);

    Bytes response_buffer(expected_data_size);
    int currentLength = really_recieved - 10;

    bytes.pointTo(10);
    bytes.getBytes(response_buffer);

    log.v("Bytes: {}", bytes);
    log.v("respo: {}", response_buffer);

    while (currentLength < expected_data_size) {
        // receive again
        if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
                bytes.getRaw(),
                array_len, &really_recieved, TIMEOUT);
            err != 0) {
            throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
                libusb_error_name(err), err));
        }

        bytes.setSize(really_recieved);
        response_buffer.putBytes(bytes);
        currentLength += really_recieved;
        response_buffer.setSize(currentLength);
    }

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    return response_buffer;
}

void CcidConnection::Setup()
{
    usb_interface = handle.claimInterface(USB_CLASS_CSCID, 0);
    if (usb_interface.number == -1) {
        throw new std::runtime_error("Failure finding correct CCID interface");
    }

    if (libusb_kernel_driver_active(*handle, usb_interface.number) == 1) {
        if (libusb_detach_kernel_driver(*handle, usb_interface.number) == 0) {
            log.v("detached kernel driver");
        }
    }

    if (auto error = libusb_claim_interface(*handle, usb_interface.number); error != 0) {
        log.e("Failure claiming CCID interface: {}({})", libusb_error_name(error), error);
        throw new std::runtime_error("Failure claiming CCID interface");
    }
}

} // namespace authpp
