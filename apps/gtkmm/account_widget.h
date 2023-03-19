#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>

class AccountWidget : public Gtk::Box {
public:
    AccountWidget();

    ~AccountWidget();

    void setName(const Glib::ustring& newName);

    void setCode(const Glib::ustring& newCode);

private:
    Gtk::Label name;
    Gtk::Label code;
};
