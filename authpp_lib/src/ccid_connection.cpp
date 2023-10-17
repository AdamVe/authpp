#include "ccid_connection.h"

#include <libusb-1.0/libusb.h>

#include "apdu.h"
#include "byte_buffer.h"
#include "logger.h"
#include "message.h"
#include "usb_device.h"
#include "util.h"

#include "fmt/fmt_byte_buffer.h"
#include "fmt/fmt_message.h"

namespace authpp {

#define TIMEOUT 1000

namespace {
    Logger log("CcidConnection");
}

CcidConnection::CcidConnection(const UsbDevice::Connection& handle)
    : handle(handle)
    , usbInterface()
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
    data.push_back(DataPair { tag, buffer });
}

ByteBuffer Response::operator[](int i) const
{
    if (i < size()) {
        return data[i].buffer;
    }

    return {};
}

uint8_t Response::tag(int i) const
{
    if (i < size()) {
        return data[i].tag;
    }

    return 0x00;
}

//
using sw_t = std::uint16_t;

sw_t getSw(const ByteBuffer& buffer)
{

    if (buffer.size() < 2) {
        log.e("Invalid data: size < 2");
        return 0x0000;
    }

    std::size_t lastIndex = buffer.size() - 1;
    return buffer.get<uint8_t>(lastIndex - 1) << 8 | buffer.get<uint8_t>(lastIndex);
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
        std::integral auto tag = buffer.get<uint8_t>(i);
        std::integral auto length = buffer.get<uint8_t>(i + 1);
        ByteBuffer data = buffer.get(i + 2, length);
        response.put(tag, data);

        // log.d("Parsed tag {:02x} with data {}", tag, data);

        i += length + 2;
    }
    return response;
}

Response CcidConnection::send(const Apdu& instruction) const
{
    Message ccidMessage(0x6f, instruction.get());
    auto buffer = transcieve(ccidMessage);

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
            buffer.put(remaining);
        }
    } else if (isAuthRequired(sw)) {
        log.e("Auth required");
    }

    return parse(buffer);
}

ByteBuffer CcidConnection::transcieve(const Message& message, int* transferred) const
{
    int really_written = 0;
    if (int err = libusb_bulk_transfer(*handle, usbInterface.endPointOut, message.get().array(),
            message.get().size(), &really_written, TIMEOUT);
        err != 0) {
        throw std::runtime_error(
            fmt::format("Failed to send data: {} {}", libusb_error_name(err), err));
    };
    log.v("send {}", message);

    // receive
    int reallyReceived = 0;
    std::size_t arrayLen = usbInterface.maxPacketSizeIn;
    ByteBuffer buffer(arrayLen);
    buffer.setByteOrder(std::endian::little);

    bool needMoreTime;
    do {
        needMoreTime = false;
        if (int err = libusb_bulk_transfer(*handle, usbInterface.endPointIn, buffer.array(),
                arrayLen, &reallyReceived, TIMEOUT);
            err != 0) {
            throw std::runtime_error(
                fmt::format("Failed to receive data: {} {}", libusb_error_name(err), err));
        };
        needMoreTime = (buffer.get<uint8_t>(7) & 0x80) == 0x80;
    } while (needMoreTime);

    buffer.setSize(reallyReceived);

    // size of data
    uint32_t expectedDataSize = buffer.get<uint16_t>(1);
    int currentLength = reallyReceived - 10;

    ByteBuffer responseBuffer = buffer.get(10, currentLength);
    responseBuffer.setByteOrder(std::endian::little).setSize(expectedDataSize);
    while (currentLength < expectedDataSize) {
        // receive again
        if (int err = libusb_bulk_transfer(*handle, usbInterface.endPointIn, buffer.array(),
                arrayLen, &reallyReceived, TIMEOUT);
            err != 0) {
            throw std::runtime_error(
                fmt::format("Failed to receive data: {} {}", libusb_error_name(err), err));
        }

        buffer.setSize(reallyReceived);
        responseBuffer.pointTo(currentLength);
        responseBuffer.put(buffer);
        currentLength += reallyReceived;
    }

    if (transferred != nullptr) {
        *transferred = reallyReceived;
    }

    log.v("recv {}", responseBuffer);
    return responseBuffer;
}

void CcidConnection::setup()
{
    usbInterface = handle.claimInterface(USB_CLASS_CSCID, 0);
    if (usbInterface.number == -1) {
        throw std::runtime_error("Failure finding correct CCID interface");
    }

    if (libusb_kernel_driver_active(*handle, usbInterface.number) == 1) {
        if (libusb_detach_kernel_driver(*handle, usbInterface.number) == 0) {
            log.v("detached kernel driver");
        }
    }

    if (auto error = libusb_claim_interface(*handle, usbInterface.number); error != 0) {
        log.e("Failure claiming CCID interface: {}({})", libusb_error_name(error), error);
        throw std::runtime_error("Failure claiming CCID interface");
    }
}

} // authpp
