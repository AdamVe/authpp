#include <CcidConnection.h>

#include <Apdu.h>
#include <ByteArray.h>
#include <Formatters.h>
#include <Message.h>
#include <UsbDeviceHandle.h>
#include <Util.h>

#include <libusb-1.0/libusb.h>

namespace authpp {

#define TIMEOUT 10000

CcidConnection::CcidConnection(const UsbDeviceHandle& handle)
    : log("CcidConnection")
    , handle(handle)
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
    if (int err = libusb_bulk_transfer(*handle, endpoint_out, (unsigned char*)message.get(),
            message.size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message);

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

    byteArray.setDataSize(really_recieved);

    log.v("recv {}", byteArray);

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
