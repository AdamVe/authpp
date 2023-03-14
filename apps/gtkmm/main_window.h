#pragma once

#include <gtkmm.h>

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
    Gtk::ListView accountList;
    Gtk::Box boxButtons;
    Gtk::Button buttonRefresh;

    Glib::RefPtr<Gtk::StringList> stringList;
};

} // namespace authppgtk
