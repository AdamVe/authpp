#pragma once

#include "byte_buffer.h"
#include "usb_device.h"

#define APDU_SUCCESS 0x9000
#define APDU_MORE_DATA 0x6100
#define APDU_AUTH_REQUIRED 0x6982
#define APDU_NO_SPACE 0x6a84
#define APDU_WRONG_SYNTAX 0x6a80
#define APDU_GENERIC_ERROR 0x6581

namespace authpp {

class Message;
class Apdu;

class Response {
public:
    std::size_t size() const;

    void put(uint8_t tag, const ByteBuffer& buffer);
    ByteBuffer operator[](int i) const;

private:
    struct DataPair {
        uint8_t tag;
        ByteBuffer buffer;
    };

    std::vector<DataPair> data;
};

class CcidConnection {
public:
    explicit CcidConnection(const UsbDevice::Connection& handle);
    virtual ~CcidConnection();

    Response send(const Apdu& apdu) const;

private:
    void setup();
    Response parse(const ByteBuffer& buffer) const;
    ByteBuffer transcieve(const Message& message, int* transferred = nullptr) const;

    const UsbDevice::Connection& handle;
    UsbDevice::Interface usb_interface;

    std::byte slot { 1 };
    std::byte sequence { 1 };

    const int USB_CLASS_CSCID { 0x0b };
};

} // namespace authpp
