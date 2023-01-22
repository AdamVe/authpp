#include <CcidConnection.h>

#include <ByteArray.h>
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
    int len = 0;
    auto atr = transcieve(Message((std::byte)0x62, nullptr, 0), &len);
    Log.v("ATR: {}", util::byteDataToString(atr.get(), len));
}

CcidConnection::~CcidConnection() = default;

template <typename T>
ByteArray CcidConnection::transcieve(T&& message, int* transferred) const
{
    log.d("Message: {}", message.toString());

    // setup
    int currentConfiguration = 0;
    if (int err = libusb_get_configuration(*handle, &currentConfiguration); err != 0) {
        log.e("Failed to receive configuration {}", err);
    } else {
        log.d("Current configuration {}", currentConfiguration);
    }

    if (libusb_kernel_driver_active(*handle, interface_ccid) == 1) {
        log.d("Kernel driver was active for interface {}", interface_ccid);
        if (libusb_detach_kernel_driver(*handle, interface_ccid) == 0) {
            log.d("Kernel Driver Detached!");
        }
    } else {
        log.d("Kernel driver was not active for interface {}", interface_ccid);
    }

    if (libusb_claim_interface(*handle, interface_ccid) != 0) {
        log.d("Cannot Claim Interface");
        return {};
    } else {
        log.d("Claimed Interface {}", interface_ccid);
    }

    // send
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, endpoint_out, (unsigned char*)message.get(),
            message.size(), &really_written, TIMEOUT);
        err != 0) {
        log.e("Failed to send data: {} {}", libusb_error_name(err),
            err);

        return {};
    };
    log.d("Sent {} bytes", message.size());

    // receive
    int really_recieved = 0;
    std::size_t array_len = 64;
    ByteArray byteArray(64);
    if (int err = libusb_bulk_transfer(*handle, endpoint_in, (unsigned char*)byteArray.get(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        log.e("Failed to receive data: {} {}", libusb_error_name(err),
            err);
        return {};
    };

    log.v("After read: array_len={} byteArray.len={} transferred={}", array_len, byteArray.getSize(), really_recieved);

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    return byteArray;
}

} // namespace authpp
