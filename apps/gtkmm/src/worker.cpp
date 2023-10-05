#include "worker.h"
#include "app_window.h"

#include <libauthpp/oath_session.h>
#include <libauthpp/oath_session_helper.h>
#include <libauthpp/time_util.h>
#include <libauthpp/usb_manager.h>

namespace authppgtk {
namespace {
    authpp::Logger log("Worker");
}

Worker::Worker()
    : m_mutex()
    , m_devices()
    , m_accounts()
{
}

void Worker::run(authppgtk::AppWindow* appWindow)
{
    using namespace authpp;
    using namespace authpp::oath;
    using namespace std::chrono_literals;

    log.d("Worker thread started");

    UsbManager usbManager;

    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    while (!m_stopped) {

        std::this_thread::sleep_for(300ms);

        auto devices = usbManager.pollUsbDevices(matchVendorYubico, 0);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (devices.size() != m_devices.size()) {
                m_devices = std::move(devices);
                log.d("Device count: {}", m_devices.size());
                appWindow->notify_device_change();
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_accounts_request) {
                m_accounts_request = false;
                auto currentSeconds = TimeUtil::getCurrentSeconds();
                log.d("Worker thread: accounts request at time {}s", currentSeconds);

                std::vector<Credential> accounts;
                for (auto&& device : m_devices) {
                    auto calculated = calculateAll(device, currentSeconds);
                    accounts.insert(accounts.end(), calculated.begin(), calculated.end());
                }
                m_accounts = accounts;

                appWindow->notify_accounts_change();
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop_request) {
                log.d("Worker thread: stop request");
                m_stopped = true;
                m_stop_request = false;
            }
        }
    }

    log.d("Worker thread finished");
}

void Worker::requestAccounts()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_accounts_request = true;
}

void Worker::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop_request = true;
}

const std::vector<authpp::oath::Credential>& Worker::getAccounts() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_accounts;
}

const std::vector<authpp::UsbDevice>& Worker::getDevices() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_devices;
}

} // namespace authppgtk