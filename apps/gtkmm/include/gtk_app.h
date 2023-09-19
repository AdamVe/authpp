#pragma once

#include <gtkmm.h>

#include "oath_session.h"

namespace authppgtk {

class GtkApp {
public:
    GtkApp();
    virtual ~GtkApp();

    Gtk::Window& getAppWindow() { return *appWindow; }

protected:
    void onButtonRefresh();
    void onSetupItem(const Glib::RefPtr<Gtk::ListItem>& list_item);
    void onBindItem(const Glib::RefPtr<Gtk::ListItem>& list_item);

    Gtk::Window* appWindow;

    Glib::RefPtr<Gtk::StringList> stringList;
    std::vector<authpp::oath::Credential> accountList;
};

} // namespace authppgtk
