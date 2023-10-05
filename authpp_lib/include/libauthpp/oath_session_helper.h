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
    oath::Session oath_session(conn);
    if (!pwd.empty()) {
        oath_session.unlock(pwd);
    }
    return f(oath_session);
}

std::vector<Credential> calculateAll(const UsbDevice& key, long secondsSinceEpoch);

std::vector<Credential> listCredentials(const UsbDevice& key);

Credential calculate(const UsbDevice& key, long secondsSinceEpoch, std::string_view name);

} // namespace authpp::oath
