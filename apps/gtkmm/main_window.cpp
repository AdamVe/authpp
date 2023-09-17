#include "main_window.h"

#include <vector>

#include <giomm/liststore.h>

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
} // namespace

MainWindow::MainWindow()
    : verticalBox(Gtk::Orientation::VERTICAL)
    , buttonRefresh("Refresh")
{
    set_title("Authpp Gtk");
    set_default_size(250, 600);

    verticalBox.set_margin(8);
    set_child(verticalBox);

    scrolledWindow.set_child(accountListView);

    scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolledWindow.set_expand();

    verticalBox.append(scrolledWindow);
    verticalBox.append(boxButtons);

    boxButtons.append(buttonRefresh);
    boxButtons.set_margin(5);
    buttonRefresh.set_hexpand(true);
    buttonRefresh.set_halign(Gtk::Align::CENTER);
    buttonRefresh.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onButtonRefresh));

    stringList = Gtk::StringList::create({});

    auto selection_model = Gtk::SingleSelection::create(stringList);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    accountListView.set_model(selection_model);

    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::mem_fun(*this, &MainWindow::onSetupLabel));
    factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::onBindName));
    accountListView.set_factory(factory);

    onButtonRefresh();
}

MainWindow::~MainWindow()
= default;

void MainWindow::onButtonRefresh()
{
    accountList = getAccounts();
    std::vector<Glib::ustring> accountNames;
    for (auto&& account : accountList) {
        accountNames.emplace_back(account.name);
    }
    stringList->splice(0, stringList->get_n_items(), accountNames);
}

void MainWindow::onSetupLabel(const Glib::RefPtr<Gtk::ListItem>& list_item) // NOLINT(*-convert-member-functions-to-static)
{
    list_item->set_child(*Gtk::make_managed<AccountWidget>());
}

void MainWindow::onBindName(const Glib::RefPtr<Gtk::ListItem>& list_item)
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
