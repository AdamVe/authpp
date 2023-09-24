#include "worker.h"
#include "app_window.h"

#include <libauthpp/oath_session.h>
#include <libauthpp/oath_session_helper.h>
#include <libauthpp/usb_manager.h>

namespace authppgtk {

void Worker::requestAccounts(authppgtk::AppWindow* appWindow)
{
    using namespace authpp;
    using namespace authpp::oath;

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    std::vector<Credential> accounts;
    auto keys = usbManager.pollUsbDevices(matchVendorYubico, 0);
    if (!keys.empty()) {
        auto calculated = calculateAll(keys[0]);
        accounts.insert(accounts.end(), calculated.begin(), calculated.end());
    }
    m_accounts = accounts;

    appWindow->notify();
}

void Worker::getAccounts(std::vector<authpp::oath::Credential>& accounts) { accounts = m_accounts; }

} // namespace authppgtk