#pragma once

#include "ByteArray.h"

namespace authpp {

class UsbDeviceHandle;
class Message;

class CcidConnection {
public:
    explicit CcidConnection(const UsbDeviceHandle& handle);
    virtual ~CcidConnection();

    template <typename T>
    ByteArray transcieve(T&&, int* transferred) const;

private:
    void setup() const;

    const UsbDeviceHandle& handle;

    std::byte slot { 1 };
    std::byte sequence { 1 };

    const int configuration { 1 };
    const int interface_ccid { 1 };
    const unsigned char endpoint_out { 0x02 };
    const unsigned char endpoint_in { 0x82 };
};

} // namespace authpp
