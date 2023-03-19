#pragma once

#include <gtkmm.h>

#include "oath_session.h"

namespace authppgtk {

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    ~MainWindow() override;

protected:
    void onButtonRefresh();
    void onSetupLabel(const Glib::RefPtr<Gtk::ListItem>& list_item);
    void onBindName(const Glib::RefPtr<Gtk::ListItem>& list_item);

    Gtk::Box verticalBox;
    Gtk::ScrolledWindow scrolledWindow;
    Gtk::ListView accountListView;
    Gtk::Box boxButtons;
    Gtk::Button buttonRefresh;

    Glib::RefPtr<Gtk::StringList> stringList;
    std::vector<authpp::oath::Credential> accountList;
};

} // namespace authppgtk
