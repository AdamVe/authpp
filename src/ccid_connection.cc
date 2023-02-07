#include "ccid_connection.h"

#include "apdu.inc"
#include "byte_array.h"
#include "formatters.h"
#include "message.h"
#include "usb_device.h"
#include "util.h"

#include <libusb-1.0/libusb.h>

namespace authpp {

#define TIMEOUT 10000

namespace {
    Logger log("CcidConnection");
}

CcidConnection::CcidConnection(const UsbDevice::Connection& handle)
    : handle(handle)
    , usb_interface()
{
    log.v("CCID connection opened");
    setup();
    auto atr { transcieve(Message((std::byte)0x62, ByteArray(0))) };
    log.v("ATR: {}", atr);
}

CcidConnection::~CcidConnection()
{
    log.v("CCID connection closed");
}

template <typename T>
ByteArray CcidConnection::transcieve(T&& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_out, (unsigned char*)message.get(),
            message.size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message);

    // receive
    int really_recieved = 0;
    std::size_t array_len = usb_interface.max_packet_size_in;
    ByteArray byteArray(array_len);
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in, (unsigned char*)byteArray.get(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to receive data: {} {}", libusb_error_name(err),
            err));
    };

    byteArray.setDataSize(really_recieved);
    log.v("recv {}", byteArray);

    // size of data
    int expectedDataSize;
    expectedDataSize = (unsigned)byteArray.get()[1] | (unsigned)byteArray.get()[2] << 8 | (unsigned)byteArray.get()[3] << 16 | (unsigned)byteArray.get()[4] << 24;

    ByteArray responseBuffer(expectedDataSize);
    int currentLength = really_recieved - 10;

    memcpy(responseBuffer.get(), byteArray.get() + 10, currentLength);
    responseBuffer.setDataSize(really_recieved - 10);

    while (currentLength < expectedDataSize) {
        // receive again
        if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in, (unsigned char*)byteArray.get(), array_len, &really_recieved, TIMEOUT);
            err != 0) {
            throw new std::runtime_error(fmt::format("Failed to receive data: {} {}", libusb_error_name(err), err));
        }

        byteArray.setDataSize(really_recieved);
        memcpy(responseBuffer.get() + currentLength, byteArray.get(), really_recieved);
        currentLength += really_recieved;
        responseBuffer.setDataSize(currentLength);
    }

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    return responseBuffer;
}

void CcidConnection::setup()
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

    if (libusb_claim_interface(*handle, usb_interface.number) != 0) {
        throw new std::runtime_error("Failure claiming CCID interface");
    }
}

} // namespace authpp
