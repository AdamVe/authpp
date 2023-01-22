#include <CcidConnection.h>

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
}

CcidConnection::~CcidConnection() = default;

std::vector<byte> CcidConnection::transcieve(byte* data, std::size_t dataLength) const
{
    log.d("Transcieve");

    auto m = Message(0x62, nullptr, 0);
    log.d("Message: {}", m.toString());

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
    int transferred = 0;
    if (int err = libusb_bulk_transfer(*handle, endpoint_out, m.get(),
            m.size(), &transferred, TIMEOUT);
        err != 0) {
        log.e("Failed to send data: {} {}", libusb_error_name(err),
            err);

        return {};
    };
    log.d("Sent {} bytes", dataLength);

    // receive
    std::size_t buffer_size = 64;
    byte buffer[buffer_size];
    if (int err = libusb_bulk_transfer(*handle, endpoint_in, buffer,
            buffer_size, &transferred, TIMEOUT);
        err != 0) {
        log.e("Failed to receive data: {} {}", libusb_error_name(err),
            err);
        return {};
    };
    log.d("Received {} bytes: {}", transferred, util::byteDataToString(buffer, transferred));

    return {};
}

} // namespace authpp
