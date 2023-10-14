#pragma once

#include <libauthpp/oath_session.h>

#include "timer.h"

#include <vector>

namespace authppgtk {
class AppWindow;
class Worker {
public:
    Worker();

    void run(AppWindow*);
    void requestAccounts();
    void stop();

    const std::vector<authpp::UsbDevice>& getDevices() const;
    const std::vector<authpp::oath::Credential>& getAccounts() const;

private:
    mutable std::mutex m_mutex;

    std::vector<authpp::UsbDevice> m_devices;
    std::vector<authpp::oath::Credential> m_accounts;
    Timer m_refresh_timer;
    bool m_stopped { false };
    bool m_stop_request { false };
    bool m_accounts_request { false };
};

} // namespace authppgtk
