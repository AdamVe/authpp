#include "ccid_connection.h"

#include <libusb-1.0/libusb.h>

#include "apdu.h"
#include "logger.h"
#include "message.h"
#include "usb_device.h"
#include "util.h"

#include "fmt/fmt_byte_buffer.h"
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
    setup();
    auto atr { transcieve(Message(0x62, {})) };
    log.v("ATR: {}", atr);
}

CcidConnection::~CcidConnection() { log.v("CCID connection closed"); }

std::size_t Response::size() const { return data.size(); }

void Response::put(uint8_t tag, const ByteBuffer& buffer)
{
    data.emplace_back(tag, buffer);
}

ByteBuffer Response::getByIndex(int i) const
{
    if (i < size()) {
        return data[i].buffer;
    }

    return {};
}

//
using sw_t = std::uint16_t;

sw_t getSw(const ByteBuffer& buffer)
{

    if (buffer.size() < 2) {
        log.e("Invalid data: size < 2");
        return 0x0000;
    }

    std::size_t last_index = buffer.size() - 1;
    return buffer.getByte(last_index - 1) << 8 | buffer.getByte(last_index);
}

bool isSuccess(uint16_t sw)
{
    return sw == APDU_SUCCESS;
}

bool isMoreData(uint16_t sw)
{
    return sw >> 8 == 0x61;
}

bool isAuthRequired(uint16_t sw)
{
    return sw == APDU_AUTH_REQUIRED;
}
//

Response CcidConnection::parse(const ByteBuffer& buffer) const
{
    Response response;
    int32_t i = 0;
    while (i < static_cast<int32_t>(buffer.size()) - 2) {
        auto tag = buffer.getByte(i);
        auto length = buffer.getByte(i + 1);
        ByteBuffer data = buffer.getBytes(i + 2, length);
        response.put(tag, data);

        // log.d("Parsed tag {:02x} with data {}", tag, data);

        i += length + 2;
    }
    return response;
}

Response CcidConnection::send(const Apdu& instruction) const
{
    Message ccid_message(0x6f, instruction.get());
    auto buffer = transcieve(ccid_message);

    auto sw { getSw(buffer) };
    if (isSuccess(sw)) {
        // success
    } else if (isMoreData(sw)) {
        buffer.setSize(buffer.size() - 2);
        while (isMoreData(sw)) {
            Message send_remaining(0x6f, ByteBuffer { 0x00, 0xa5, 0x00, 0x00 });
            auto remaining = transcieve(send_remaining);
            sw = getSw(remaining);
            remaining.setSize(remaining.size() - 2);
            auto currentSize = buffer.size();
            buffer.setSize(buffer.size() + remaining.size());
            buffer.pointTo(currentSize);
            buffer.putBytes(remaining);
        }
    } else if (isAuthRequired(sw)) {
        log.e("Auth required");
    }

    return parse(buffer);
}

ByteBuffer CcidConnection::transcieve(const Message& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_out,
            message.get().array(),
            message.get().size(), &really_written, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(
            fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message);

    // receive
    int really_recieved = 0;
    std::size_t array_len = usb_interface.max_packet_size_in;
    ByteBuffer buffer(array_len);
    if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
            buffer.array(),
            array_len, &really_recieved, TIMEOUT);
        err != 0) {
        throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
            libusb_error_name(err), err));
    };

    buffer.setSize(really_recieved);

    // size of data
    uint32_t expected_data_size = buffer.getInt(1);
    int currentLength = really_recieved - 10;

    ByteBuffer response_buffer = buffer.getBytes(10, currentLength);
    response_buffer.setSize(expected_data_size);
    while (currentLength < expected_data_size) {
        // receive again
        if (int err = libusb_bulk_transfer(*handle, usb_interface.endpoint_in,
                buffer.array(),
                array_len, &really_recieved, TIMEOUT);
            err != 0) {
            throw new std::runtime_error(fmt::format("Failed to receive data: {} {}",
                libusb_error_name(err), err));
        }

        buffer.setSize(really_recieved);
        response_buffer.pointTo(currentLength);
        response_buffer.putBytes(buffer);
        currentLength += really_recieved;
    }

    if (transferred != nullptr) {
        *transferred = really_recieved;
    }

    log.v("recv {}", response_buffer);
    return response_buffer;
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

    if (auto error = libusb_claim_interface(*handle, usb_interface.number); error != 0) {
        log.e("Failure claiming CCID interface: {}({})", libusb_error_name(error), error);
        throw new std::runtime_error("Failure claiming CCID interface");
    }
}

} // namespace authpp
