#include "account_holder.h"

namespace authppgtk {
using Account = authpp::oath::Credential;
auto AccountHolder::create(Account account) -> Glib::RefPtr<AccountHolder>
{
    return Glib::make_refptr_for_instance<AccountHolder>(new AccountHolder(std::move(account)));
}

AccountHolder::AccountHolder(Account account)
    : Glib::ObjectBase(typeid(AccountHolder))
    , Glib::Object()
    , account(std::move(account))
{
}
} // namespace authppgtk
