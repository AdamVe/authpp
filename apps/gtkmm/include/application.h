#pragma once

#include <gtkmm/application.h>

namespace authppgtk {

class AppWindow;

class Application : public Gtk::Application {
protected:
    Application();

public:
    static Glib::RefPtr<Application> create();

protected:
    void on_activate() override;
    void on_startup() override;

private:
    AppWindow* create_appwindow();
    void on_hide_window(Gtk::Window* window);

    void on_action_about();
    void on_action_quit();
};

} // namespace authppgtk