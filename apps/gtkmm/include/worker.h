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
    void requestStop();

    const std::vector<authpp::UsbDevice>& getDevices() const;
    const std::vector<authpp::oath::Credential>& getAccounts() const;

private:
    mutable std::mutex mutex;

    std::vector<authpp::UsbDevice> devices;
    std::vector<authpp::oath::Credential> accounts;
    Timer refreshTimer;
    bool isStopped { false };
    bool stopRequested { false };
    bool accountsRequested { false };
};

} // authppgtk
