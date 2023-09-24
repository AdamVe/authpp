#pragma once

#include "account_holder.h"
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
    AppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

    static AppWindow* create();

    void notify();

private:
    void onButtonRefresh();

    void onWorkerNotify();

    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Button* refreshButton;
    Gtk::Button* refreshButtonWithSpinner;
    Glib::RefPtr<AccountHolder> accountHolder;
    Glib::RefPtr<Gio::ListStore<AccountHolder>> accountModel;

    Glib::Dispatcher dispatcher;
    Worker worker;
    std::thread* workerThread;
};

} // namespace authppgtk
