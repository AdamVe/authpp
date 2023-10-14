#include "worker.h"
#include "app_window.h"
#include "timer.h"

#include <libauthpp/oath_session.h>
#include <libauthpp/oath_session_helper.h>
#include <libauthpp/time_util.h>
#include <libauthpp/usb_manager.h>

namespace authppgtk {
using namespace authpp;
namespace {
    authpp::Logger log("Worker");
}

Worker::Worker()
    : mutex()
    , devices()
    , accounts()
    , refreshTimer([this]() {
        std::lock_guard<std::mutex> lock(mutex);
        auto currentMs = TimeUtil::getCurrentMilliSeconds();
        log.d("Passed refresh time (current: {})", TimeUtil::toString(currentMs));
        accountsRequested = true;
        return false;
    })
{
}

void Worker::run(authppgtk::AppWindow* appWindow)
{
    using namespace authpp::oath;
    using namespace std::chrono_literals;

    log.d("Worker thread started");

    UsbManager usbManager;
    auto matchVendorYubico = [](auto&& descriptor) {
        const auto VENDOR_YUBICO = 0x1050;
        return descriptor.idVendor == VENDOR_YUBICO;
    };

    while (!isStopped) {

        std::this_thread::sleep_for(300ms);

        auto polledDevices = usbManager.pollUsbDevices(matchVendorYubico, 0);
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (polledDevices.size() != devices.size()) {
                devices = std::move(polledDevices);
                log.d("Device count: {}", devices.size());
                appWindow->notify_device_change();
                refreshTimer.cancel();
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex);
            if (accountsRequested) {
                accountsRequested = false;
                refreshTimer.cancel();
                auto currentMillis = TimeUtil::getCurrentMilliSeconds();
                auto currentSeconds = currentMillis / 1000;
                log.d("Worker thread: accounts request at time {}",
                    TimeUtil::toString(currentMillis));

                std::vector<Credential> retrievedAccounts;
                long delay = authpp::TimeUtil::DEFAULT_TOTP_INTERVAL;
                for (auto&& device : devices) {
                    auto calculated = calculateAll(device, currentSeconds);

                    for (auto&& account : calculated) {
                        auto timeStep = TimeUtil::getTotpTimeStep(currentSeconds, account.timeStep);
                        auto accountDelay = (timeStep + 1) * account.timeStep - currentSeconds;
                        delay = std::min(delay, accountDelay);
                        retrievedAccounts.push_back(account);
                    }
                }
                accounts = retrievedAccounts;

                refreshTimer.schedule(delay * 1000);

                appWindow->notify_accounts_change();
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex);
            if (stopRequested) {
                log.d("Worker thread: cancel request");
                refreshTimer.cancel();
                isStopped = true;
                stopRequested = false;
            }
        }
    }

    log.d("Worker thread finished");
}

void Worker::requestAccounts()
{
    std::lock_guard<std::mutex> lock(mutex);
    accountsRequested = true;
}

void Worker::requestStop()
{
    std::lock_guard<std::mutex> lock(mutex);
    stopRequested = true;
}

const std::vector<authpp::oath::Credential>& Worker::getAccounts() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return accounts;
}

const std::vector<authpp::UsbDevice>& Worker::getDevices() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return devices;
}

} // authppgtk