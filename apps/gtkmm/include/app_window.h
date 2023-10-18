#pragma once

#include "account_holder.h"
#include "timer.h"
#include "timer_widget.h"
#include "worker.h"

#include <giomm/liststore.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>

#include <thread>

namespace Gtk {
class Builder;
class Button;
}

namespace authppgtk {
class AppWindow : public Gtk::ApplicationWindow {
public:
    AppWindow(BaseObjectType* baseObjectType, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    ~AppWindow() override;

    static AppWindow* create();

    void notify_device_change() const;
    void notify_accounts_change() const;

private:
    void requestAccounts();

    void onDeviceChange();
    void onAccountsChange();

    void onShow();
    void onHide();
    bool onCloseRequest();

    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Button* refreshButton;
    Gtk::Button* refreshButtonWithSpinner;
    Glib::RefPtr<AccountHolder> accountHolder;
    Glib::RefPtr<Gio::ListStore<AccountHolder>> accountModel;

    Glib::Dispatcher signalDevicesChange;
    Glib::Dispatcher signalAccountsChange;
    Worker worker;
    std::thread* workerThread;
    TimerWidget timerWidget;
};

} // authppgtk
