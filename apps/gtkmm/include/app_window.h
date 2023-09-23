#pragma once

#include "account_holder.h"

#include <giomm/liststore.h>
#include <gtkmm/applicationwindow.h>

namespace Gtk {
class Builder;
}

namespace authppgtk {
class AppWindow : public Gtk::ApplicationWindow {
public:
    AppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

    static AppWindow* create();

protected:
    Glib::RefPtr<Gtk::Builder> refBuilder;
    void onButtonRefresh();

private:
    Glib::RefPtr<AccountHolder> accountHolder;
    Glib::RefPtr<Gio::ListStore<AccountHolder>> accountModel;
};

} // namespace authppgtk
