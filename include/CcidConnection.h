#pragma once

#include <Byte.h>
#include <Logger.h>

#include <vector>

namespace authpp {

class UsbDeviceHandle;

class CcidConnection {
  public:
    CcidConnection(const UsbDeviceHandle& handle);
    virtual ~CcidConnection();

    std::vector<byte> transcieve(byte *data, std::size_t dataLength) const;

  private:
    Logger log;
    const UsbDeviceHandle& handle;

    byte slot {1};
    byte sequence {1};

    const int configuration{1};
    const int interface_ccid{1};
    const unsigned char endpoint_out{0x02};
    const unsigned char endpoint_in{0x82};
};

} // namespace authpp
