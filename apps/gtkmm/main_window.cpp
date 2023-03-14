#include "main_window.h"

#include <functional>
#include <vector>

#include "oath_session.h"
#include "time_util.h"
#include "usb_device.h"
#include "usb_manager.h"

using namespace authpp;

namespace authppgtk {

namespace {
    std::string pwd;

    template <typename T>
    T useOathSession(const UsbDevice& key, std::function<T(oath::Session&)> f)
    {
        UsbDevice::Connection usbConnection(key);
        CcidConnection conn(usbConnection);
        oath::Session oath_session(conn);
        if (!pwd.empty()) {
            oath_session.unlock(pwd);
        }
        return f(oath_session);
    }

    std::vector<oath::Credential> calculateAll(const UsbDevice& key)
    {
        return useOathSession<std::vector<oath::Credential>>(key, [](auto& session) {
            auto credentials = session.calculateAll(TimeUtil::getTimeStep());
#ifdef __cpp_lib_ranges
            std::ranges::sort(credentials, oath::Credential::compareByName);
#else
        std::sort(credentials.begin(), credentials.end(), oath::Credential::compareByName);
#endif
            return credentials;
        });
    }

    std::vector<oath::Credential> getAccounts()
    {

        UsbManager usbManager;

        auto matchVendorYubico = [](auto&& descriptor) {
            const auto VENDOR_YUBICO = 0x1050;
            return descriptor.idVendor == VENDOR_YUBICO;
        };

        std::vector<oath::Credential> accounts;
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

    scrolledWindow.set_child(accountList);

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
    accountList.set_model(selection_model);

    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::mem_fun(*this, &MainWindow::onSetupLabel));
    factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::onBindName));
    accountList.set_factory(factory);

    onButtonRefresh();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onButtonRefresh()
{
    auto accounts = getAccounts();
    std::vector<Glib::ustring> accountInfos;
    std::transform(accounts.cbegin(),
        accounts.cend(),
        std::back_insert_iterator(accountInfos),
        [](auto&& account) {
            return account.name + " " + account.code.value;
        });
    stringList->splice(0, stringList->get_n_items(), accountInfos);
}

void MainWindow::onSetupLabel(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", Gtk::Align::START));
}

void MainWindow::onBindName(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto pos = list_item->get_position();
    if (pos == GTK_INVALID_LIST_POSITION)
        return;
    auto label = dynamic_cast<Gtk::Label*>(list_item->get_child());
    if (!label)
        return;
    label->set_text(stringList->get_string(pos));
}
} // namespace authppgtk
