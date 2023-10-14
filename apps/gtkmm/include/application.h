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
    AppWindow* createAppwindow();
    void onHideWindow(Gtk::Window* window);

    void onActionAbout();
    void onActionQuit();
};

} // authppgtk