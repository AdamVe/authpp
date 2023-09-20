#pragma once

#include <gtkmm.h>

#include "account_holder.h"
#include "oath_session.h"

namespace authppgtk {

class GtkApp {
public:
    GtkApp();
    virtual ~GtkApp();

    Gtk::Window& getAppWindow() { return *appWindow; }

protected:
    void onButtonRefresh();

    Glib::RefPtr<AccountHolder> accountHolder;
    Glib::RefPtr<Gio::ListStore<AccountHolder>> accountModel;
    Gtk::Window* appWindow;
};

} // namespace authppgtk
