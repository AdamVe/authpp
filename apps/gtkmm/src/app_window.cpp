#include "app_window.h"
#include "resources.h"

#include <gtkmm.h>
#include <gtkmm/builder.h>

#include <libauthpp/oath_session.h>
#include <libauthpp/oath_session_helper.h>
#include <libauthpp/usb_manager.h>

using namespace authpp;
namespace authppgtk {

namespace {

    std::vector<oath::Credential> getAccounts()
    {
        using namespace oath;
        UsbManager usbManager;

        auto matchVendorYubico = [](auto&& descriptor) {
            const auto VENDOR_YUBICO = 0x1050;
            return descriptor.idVendor == VENDOR_YUBICO;
        };

        std::vector<Credential> accounts;
        auto keys = usbManager.pollUsbDevices(matchVendorYubico, 100);
        if (!keys.empty()) {
            auto calculated = calculateAll(keys[0]);
            accounts.insert(accounts.end(), calculated.begin(), calculated.end());
        }
        return accounts;
    }
} // namespace

AppWindow::AppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::ApplicationWindow(cobject)
    , refBuilder(refBuilder)
    , accountHolder()
    , accountModel(Gio::ListStore<AccountHolder>::create())
{
    auto refreshButton = refBuilder->get_widget<Gtk::Button>("btn_refresh");
    auto accountListView = refBuilder->get_widget<Gtk::ListView>("listview_accounts");

    auto gtkSingleSelection = Gtk::SingleSelection::create(accountModel);
    gtkSingleSelection->set_autoselect(false);
    gtkSingleSelection->set_can_unselect(true);
    accountListView->set_model(gtkSingleSelection);

    auto factory = Gtk::SignalListItemFactory::create();
    accountListView->set_factory(factory);

    factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& list_item) {
        const auto builder
            = Gtk::Builder::create_from_file(Resources::get_ui_path() / "account_widget.ui");
        auto* const accountWidget = builder->get_widget<Gtk::Box>("account");

        list_item->set_data("name", builder->get_widget<Gtk::Label>("name"));
        list_item->set_data("issuer", builder->get_widget<Gtk::Label>("issuer"));
        list_item->set_data("code", builder->get_widget<Gtk::Label>("code"));
        list_item->set_child(*accountWidget);
    });
    factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem>& list_item) {
        auto* const name = static_cast<Gtk::Label*>(list_item->get_data("name"));
        auto* const issuer = static_cast<Gtk::Label*>(list_item->get_data("issuer"));
        auto* const code = static_cast<Gtk::Label*>(list_item->get_data("code"));

        auto holder = std::dynamic_pointer_cast<AccountHolder>(list_item->get_item());

        std::string issuerValue;
        std::string nameValue;
        auto index = holder->account.name.find(':');
        if (index == std::string::npos) {
            // no issuer
            nameValue = holder->account.name;
        } else {
            issuerValue = holder->account.name.substr(0, index);
            nameValue = holder->account.name.substr(index + 1);
        }

        name->set_text(nameValue);
        issuer->set_text(issuerValue);
        code->set_text(holder->account.code.value);
    });

    refreshButton->signal_clicked().connect(sigc::mem_fun(*this, &AppWindow::onButtonRefresh));
    onButtonRefresh();
}

// static
AppWindow* AppWindow::create()
{
    auto builder = Gtk::Builder::create_from_file(Resources::get_ui_path() / "authppgtk.ui");
    auto window = Gtk::Builder::get_widget_derived<AppWindow>(builder, "win_app");
    if (!window) {
        throw std::runtime_error("No \"win_app\" object in window.ui");
    }

    return window;
}

void AppWindow::onButtonRefresh()
{
    auto accountList = getAccounts();
    std::vector<Glib::RefPtr<AccountHolder>> credentials;
    for (auto&& account : accountList) {
        credentials.push_back(AccountHolder::create(account));
    }
    accountModel->splice(0, accountModel->get_n_items(), credentials);
}

} // namespace authppgtk