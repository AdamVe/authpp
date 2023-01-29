#include "CcidConnection.h"

#include "Apdu.h"
#include "ByteArray.h"
#include "Formatters.h"
#include "Message.h"
#include "UsbDevice.h"
#include "Util.h"

#include <libusb-1.0/libusb.h>

namespace authpp {

#define TIMEOUT 10000

CcidConnection::CcidConnection(const UsbDevice::Connection& handle)
    : handle(handle)
    , usbInterface()
{
    Log.v("CCID connection opened");
    setup();
    auto atr { transcieve(Message((std::byte)0x62, ByteArray(0)), nullptr) };
    Log.v("ATR: {}", atr);
}

CcidConnection::~CcidConnection()
{
    Log.v("CCID connection closed");
}

template <typename T>
ByteArray CcidConnection::transcieve(T&& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, usbInterface.endpoint_out, (unsigned char*)message.get(),
            message.size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    Log.v("send {}", message);

    // receive
    int really_recieved = 0;
    std::size_t array_len = usbInterface.max_packet_size_in;
    ByteArray byteArray(array_len);
    if (int err = libusb_bulk_transfer(*handle, usbInterface.endpoint_in, (unsigned char*)byteArray.get(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to receive data: {} {}", libusb_error_name(err),
            err));
    };

    byteArray.setDataSize(really_recieved);

    Log.v("recv {}", byteArray);

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    return byteArray;
}

void CcidConnection::setup()
{
    usbInterface = handle.claimInterface(USB_CLASS_CSCID, 0);
    if (usbInterface.number == -1) {
        throw new std::runtime_error("Failure finding correct CCID interface");
    }

    if (libusb_kernel_driver_active(*handle, usbInterface.number) == 1) {
        if (libusb_detach_kernel_driver(*handle, usbInterface.number) == 0) {
            Log.v("detached kernel driver");
        }
    }

    if (libusb_claim_interface(*handle, usbInterface.number) != 0) {
        throw new std::runtime_error("Failure claiming CCID interface");
    }
}

} // namespace authpp
