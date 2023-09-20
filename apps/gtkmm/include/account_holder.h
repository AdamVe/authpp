#pragma once

#include <libauthpp/oath_session.h>

#include <glibmm/object.h>
#include <glibmm/refptr.h>

namespace authppgtk {
class AccountHolder : public Glib::Object {
    using Account = authpp::oath::Credential;

public:
    Account account;

    static auto create(Account account) -> Glib::RefPtr<AccountHolder>;

protected:
    explicit AccountHolder(Account account);
};
} // namespace authppgtk