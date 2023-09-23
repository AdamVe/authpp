#include "application.h"
#include "app_window.h"
#include "resources.h"

#include <gtkmm/cssprovider.h>
#include <iostream>

namespace authppgtk {

Application::Application()
    : Gtk::Application("org.adamve.authppgtk")
{
}

Glib::RefPtr<Application> Application::create()
{
    return Glib::make_refptr_for_instance<Application>(new Application());
}

void Application::on_activate()
{
    try {
        auto appwindow = create_appwindow();
        appwindow->present();
    } catch (const Glib::Error& ex) {
        std::cerr << "ExampleApplication::on_activate(): " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "ExampleApplication::on_activate(): " << ex.what() << std::endl;
    }
}

AppWindow* Application::create_appwindow()
{
    auto appWindow = AppWindow::create();
    add_window(*appWindow);

    appWindow->signal_hide().connect(
        sigc::bind(sigc::mem_fun(*this, &Application::on_hide_window), appWindow));

    auto provider = Gtk::CssProvider::create();
    provider->load_from_path(Resources::get_ui_path() / "authppgtk.css");
    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(), provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    return appWindow;
}

void Application::on_hide_window(Gtk::Window* window) // NOLINT(*-convert-member-functions-to-static)
{
    delete window;
}

void Application::on_startup()
{
    Gtk::Application::on_startup();

    add_action("about", sigc::mem_fun(*this, &Application::on_action_about));
    add_action("quit", sigc::mem_fun(*this, &Application::on_action_quit));
    set_accel_for_action("app.quit", "<Ctrl>Q");
}
void Application::on_action_about() { }

void Application::on_action_quit()
{
    auto windows = get_windows();
    for (auto window : windows) {
        window->hide();
    }
    quit();
}

} // authppgtk