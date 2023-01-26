#include <CcidConnection.h>

#include <Apdu.h>
#include <ByteArray.h>
#include <Message.h>
#include <UsbDeviceHandle.h>
#include <Util.h>

#include <libusb-1.0/libusb.h>

namespace authpp {

#define TIMEOUT 10000

using bytes = unsigned char[];

CcidConnection::CcidConnection(const UsbDeviceHandle& handle)
    : log("CcidConnection")
    , handle(handle)
{
    int len = 0;
    Log.v("CCID connection opened");
    setup();
    auto atr = transcieve(Message((std::byte)0x62, nullptr, 0), &len);
    Log.v("ATR: {}", util::byteDataToString(atr.get(), len));

    // test select OATH app
    // appid oath = 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01
    Apdu selectOath(0x00, 0xa4, 0x04, 0x00, ByteArray(bytes { 0xa0, 0xff, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 }));
}

CcidConnection::~CcidConnection()
{
    Log.v("CCID connection closed");
}

template <typename T>
ByteArray CcidConnection::transcieve(T&& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, endpoint_out, (unsigned char*)message.get(),
            message.size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message.toString());

    // receive
    int really_recieved = 0;
    std::size_t array_len = 64;
    ByteArray byteArray(64);
    if (int err = libusb_bulk_transfer(*handle, endpoint_in, (unsigned char*)byteArray.get(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to receive data: {} {}", libusb_error_name(err),
            err));
    };

    log.v("recv {}", util::byteDataToString(byteArray.get(), really_recieved));

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    return byteArray;
}

void CcidConnection::setup() const
{
    if (libusb_kernel_driver_active(*handle, interface_ccid) == 1) {
        if (libusb_detach_kernel_driver(*handle, interface_ccid) == 0) {
            log.v("detached kernel driver");
        }
    }

    if (libusb_claim_interface(*handle, interface_ccid) != 0) {
        throw new std::runtime_error("Failure claiming CCID interface");
    }
}

} // namespace authpp
