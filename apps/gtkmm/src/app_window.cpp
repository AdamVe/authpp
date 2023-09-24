#include "app_window.h"
#include "resources.h"

#include <gtkmm.h>
#include <gtkmm/builder.h>

#include <libauthpp/oath_session.h>

#include <iostream>

using namespace authpp;
namespace authppgtk {

AppWindow::AppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::ApplicationWindow(cobject)
    , refBuilder(refBuilder)
    , refreshButton(refBuilder->get_widget<Gtk::Button>("btn_refresh"))
    , refreshButtonWithSpinner(refBuilder->get_widget<Gtk::Button>("btn_refresh_with_spinner"))
    , accountHolder()
    , accountModel(Gio::ListStore<AccountHolder>::create())
    , dispatcher()
    , worker()
    , workerThread(nullptr)
{
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

    dispatcher.connect(sigc::mem_fun(*this, &AppWindow::onWorkerNotify));

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

void AppWindow::notify() { dispatcher.emit(); }

void AppWindow::onButtonRefresh()
{
    if (!workerThread) {
        workerThread = new std::thread([this] { worker.requestAccounts(this); });
        refreshButton->set_visible(false);
        refreshButtonWithSpinner->set_visible(true);
    }
}

void AppWindow::onWorkerNotify()
{
    if (workerThread) {
        if (workerThread->joinable()) {
            workerThread->join();
        }
        delete workerThread;
        workerThread = nullptr;
    }

    auto accountList = std::vector<authpp::oath::Credential>();
    worker.getAccounts(accountList);
    std::vector<Glib::RefPtr<AccountHolder>> credentials;
    for (auto&& account : accountList) {
        credentials.push_back(AccountHolder::create(account));
    }
    accountModel->splice(0, accountModel->get_n_items(), credentials);

    refreshButton->set_visible(true);
    refreshButtonWithSpinner->set_visible(false);
}

} // namespace authppgtk