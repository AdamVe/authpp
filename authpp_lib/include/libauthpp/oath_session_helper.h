#include <string>
#include <vector>

#include "ccid_connection.h"
#include "oath_session.h"
#include "usb_device.h"
#include "usb_manager.h"

namespace authpp::oath {

inline std::string pwd;

template <typename T>
T useOathSession(const UsbDevice& key, std::function<T(oath::Session&)> f)
{
    UsbDevice::Connection usbConnection(key);
    CcidConnection conn(usbConnection);
    oath::Session oathSession(conn);
    if (!pwd.empty()) {
        oathSession.unlock(pwd);
    }
    return f(oathSession);
}

std::vector<Credential> calculateAll(const UsbDevice& key, long secondsSinceEpoch);

std::vector<Credential> listCredentials(const UsbDevice& key);

Credential calculate(const UsbDevice& key, long secondsSinceEpoch, std::string_view name);

} // authpp::oath
