#include "app_window.h"

#include <gtkmm.h>
#include <gtkmm/builder.h>

#include <libauthpp/oath_session.h>

#include <iostream>

using namespace authpp;
namespace authppgtk {

namespace {
    authpp::Logger log("AppWindow");
}

AppWindow::AppWindow(BaseObjectType* baseObjectType, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::ApplicationWindow(baseObjectType)
    , refBuilder(refBuilder)
    , refreshButton(refBuilder->get_widget<Gtk::Button>("btn_refresh"))
    , refreshButtonWithSpinner(refBuilder->get_widget<Gtk::Button>("btn_refresh_with_spinner"))
    , accountHolder()
    , accountModel(Gio::ListStore<AccountHolder>::create())
    , signalDevicesChange()
    , signalAccountsChange()
    , worker()
    , workerThread()
{
    auto accountListView = refBuilder->get_widget<Gtk::ListView>("listview_accounts");

    auto gtkSingleSelection = Gtk::SingleSelection::create(accountModel);
    gtkSingleSelection->set_autoselect(false);
    gtkSingleSelection->set_can_unselect(true);
    accountListView->set_model(gtkSingleSelection);

    auto factory = Gtk::SignalListItemFactory::create();
    accountListView->set_factory(factory);

    factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& list_item) {
        auto builder
            = Gtk::Builder::create_from_resource("/org/adamve/authppgtk/ui/account_widget.ui");
        auto* const accountWidget = builder->get_widget<Gtk::Box>("account");

        list_item->set_data("name", builder->get_widget<Gtk::Label>("name"));
        list_item->set_data("issuer", builder->get_widget<Gtk::Label>("issuer"));
        list_item->set_data("code", builder->get_widget<Gtk::Label>("code"));
        builder->get_widget<Gtk::Box>("toprow")->append(*Gtk::make_managed<TimerWidget>());
        list_item->set_child(*accountWidget);
    });
    factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem>& list_item) {
        auto* const name = static_cast<Gtk::Label*>(list_item->get_data("name"));
        auto* const issuer = static_cast<Gtk::Label*>(list_item->get_data("issuer"));
        auto* const code = static_cast<Gtk::Label*>(list_item->get_data("code"));

        auto holder = std::dynamic_pointer_cast<AccountHolder>(list_item->get_item());

        std::string issuerValue;
        std::string nameValue;
        nameValue = holder->account.name;
        issuerValue = holder->account.issuer;

        name->set_text(nameValue);
        issuer->set_text(issuerValue);
        code->set_text(holder->account.code.value);
    });

    refreshButton->signal_clicked().connect(sigc::mem_fun(*this, &AppWindow::requestAccounts));

    signalDevicesChange.connect(sigc::mem_fun(*this, &AppWindow::onDeviceChange));
    signalAccountsChange.connect(sigc::mem_fun(*this, &AppWindow::onAccountsChange));

    this->signal_show().connect(sigc::mem_fun(*this, &AppWindow::onShow));
    this->signal_hide().connect(sigc::mem_fun(*this, &AppWindow::onHide));
    this->signal_close_request().connect(sigc::mem_fun(*this, &AppWindow::onCloseRequest), false);
}

AppWindow::~AppWindow() = default;

// static
AppWindow* AppWindow::create()
{
    auto builder = Gtk::Builder::create_from_resource("/org/adamve/authppgtk/ui/authppgtk.ui");
    auto window = Gtk::Builder::get_widget_derived<AppWindow>(builder, "win_app");
    if (!window) {
        throw std::runtime_error("No \"win_app\" object in window.ui");
    }

    return window;
}

void AppWindow::notify_device_change() const { signalDevicesChange(); }

void AppWindow::notify_accounts_change() const { signalAccountsChange(); }

void AppWindow::onDeviceChange()
{
    const auto& devices = worker.getDevices();
    log.d("Device count: {}", devices.size());
    requestAccounts();
}

void AppWindow::requestAccounts()
{
    refreshButton->set_visible(false);
    refreshButtonWithSpinner->set_visible(true);
    worker.requestAccounts();
}

void AppWindow::onAccountsChange()
{
    const auto& accountList = worker.getAccounts();

    std::vector<Glib::RefPtr<AccountHolder>> credentials;
    for (auto&& account : accountList) {
        credentials.push_back(AccountHolder::create(account));
    }
    accountModel->splice(0, accountModel->get_n_items(), credentials);

    refreshButton->set_visible(true);
    refreshButtonWithSpinner->set_visible(false);
}

void AppWindow::onShow()
{
    log.d("Window onShow");
    workerThread = new std::thread([this] { worker.run(this); });
}

void AppWindow::onHide()
{
    log.d("Window onHide");
    worker.requestStop();
    if (workerThread) {
        if (workerThread->joinable()) {
            workerThread->join();
        }
        delete workerThread;
        workerThread = nullptr;
    }
}

bool AppWindow::onCloseRequest()
{
    log.d("Window onCloseRequest");
    onHide();

    return false;
}

} // authppgtk