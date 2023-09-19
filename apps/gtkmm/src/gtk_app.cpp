#include "gtk_app.h"

#include <vector>

#include <giomm/liststore.h>

#include "resources.h"

#include "oath_session.h"
#include "oath_session_helper.h"
#include "usb_manager.h"

#include "account_widget.h"

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

    Gtk::Button* refreshButton = nullptr;
} // namespace

GtkApp::GtkApp()
{
    auto refBuilder = Gtk::Builder::create();
    refBuilder->add_from_file(Resources::get_ui_path() / "authppgtk.ui");

    appWindow = refBuilder->get_widget<Gtk::Window>("win_app");
    refreshButton = refBuilder->get_widget<Gtk::Button>("btn_refresh");

    auto accountListView = refBuilder->get_widget<Gtk::ListView>("listview_accounts");

    stringList = Gtk::StringList::create({});

    auto selection_model = Gtk::SingleSelection::create(stringList);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    accountListView->set_model(selection_model);

    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::mem_fun(*this, &GtkApp::onSetupLabel));
    factory->signal_bind().connect(sigc::mem_fun(*this, &GtkApp::onBindName));
    accountListView->set_factory(factory);

    refreshButton->signal_clicked().connect(sigc::mem_fun(*this, &GtkApp::onButtonRefresh));
    onButtonRefresh();
}

GtkApp::~GtkApp() = default;

void GtkApp::onButtonRefresh()
{
    accountList = getAccounts();
    std::vector<Glib::ustring> accountNames;
    for (auto&& account : accountList) {
        accountNames.emplace_back(account.name);
    }
    stringList->splice(0, stringList->get_n_items(), accountNames);
}

void GtkApp::onSetupLabel(const Glib::RefPtr<Gtk::ListItem>& list_item) // NOLINT(*-convert-member-functions-to-static)
{
    list_item->set_child(*Gtk::make_managed<AccountWidget>());
}

void GtkApp::onBindName(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto pos = list_item->get_position();
    if (pos == GTK_INVALID_LIST_POSITION)
        return;
    auto accountWidget = dynamic_cast<AccountWidget*>(list_item->get_child());
    if (!accountWidget) {
        return;
    }
    accountWidget->setName(accountList[pos].name);
    accountWidget->setCode(accountList[pos].code.value);
}
} // namespace authppgtk
