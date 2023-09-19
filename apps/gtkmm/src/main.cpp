#include "gtk_app.h"

#include <gtkmm/application.h>

int main(int argc, char* argv[])
{
    std::unique_ptr<authppgtk::GtkApp> gtkApp;
    auto app = Gtk::Application::create("org.adamve.authppgtk");
    app->signal_activate().connect([&]() {
        gtkApp = std::make_unique<authppgtk::GtkApp>();
        auto& appWindow = gtkApp->getAppWindow();
        app->add_window(appWindow);
        appWindow.signal_hide().connect([&appWindow] () { delete &appWindow; });
        appWindow.set_visible(true);
    });
    return app->run(argc, argv);
}
