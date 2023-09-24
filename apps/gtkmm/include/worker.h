#pragma once

#include <libauthpp/oath_session.h>

#include <vector>

namespace authppgtk {
class AppWindow;
class Worker {
public:
    void requestAccounts(AppWindow*);
    void getAccounts(std::vector<authpp::oath::Credential>&);

private:
    std::vector<authpp::oath::Credential> m_accounts;
};

} // namespace authppgtk
