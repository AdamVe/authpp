#include "application.h"
#include "app_window.h"
#include "version.h"

#include <gtkmm/aboutdialog.h>
#include <gtkmm/cssprovider.h>
#include <iostream>

namespace authppgtk {
Application::Application()
    : Gtk::Application("org.adamve.authppgtk")
{
    authpp::Logger::setLevel(authpp::Logger::Level::DEBUG);
}

Glib::RefPtr<Application> Application::create()
{
    return Glib::make_refptr_for_instance<Application>(new Application());
}

void Application::on_activate()
{
    try {
        auto appwindow = createAppwindow();
        appwindow->present();
    } catch (const Glib::Error& ex) {
        std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
    }
}

AppWindow* Application::createAppwindow()
{
    auto appWindow = AppWindow::create();
    add_window(*appWindow);

    appWindow->signal_hide().connect(
        sigc::bind(sigc::mem_fun(*this, &Application::onHideWindow), appWindow));

    auto provider = Gtk::CssProvider::create();
    provider->load_from_resource("/org/adamve/authppgtk/css/style.css");
    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(), provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    return appWindow;
}

void Application::onHideWindow(Gtk::Window* window) // NOLINT(*-convert-member-functions-to-static)
{
    delete window;
}

void Application::on_startup()
{
    Gtk::Application::on_startup();

    add_action("about", sigc::mem_fun(*this, &Application::onActionAbout));
    add_action("quit", sigc::mem_fun(*this, &Application::onActionQuit));
    set_accel_for_action("app.quit", "<Ctrl>Q");
}
void Application::onActionAbout()
{
    Gtk::Window* win = get_active_window();
    if (!win) {
        return;
    }

    auto aboutDialog = new Gtk::AboutDialog();
    Glib::RefPtr<Gdk::Texture> logo
        = Gdk::Texture::create_from_resource("/org/adamve/authppgtk/icons/org.adamve.authppgtk.svg");
    aboutDialog->set_logo(logo);

    aboutDialog->set_version(VERSION);
    aboutDialog->set_authors({ "Adam Velebil" });
    aboutDialog->set_copyright("Copyright © 2023 - Adam Velebil");
    aboutDialog->set_comments("Utility for managing OATH accounts on compatible devices.");
    aboutDialog->set_website_label("GitHub repository");
    aboutDialog->set_website("https://github.com/AdamVe/authpp");
    aboutDialog->set_license_type(Gtk::License::MIT_X11);
    aboutDialog->set_program_name("Authpp Gtk");
    aboutDialog->set_hide_on_close();

    aboutDialog->signal_hide().connect(
        [aboutDialog, logo]() mutable {
            if (logo) {
                logo->unreference();
                logo = nullptr;
            }
            delete aboutDialog;
            aboutDialog = nullptr;
        },
        false);

    aboutDialog->set_modal();
    aboutDialog->set_transient_for(*win);
    aboutDialog->present();
}

void Application::onActionQuit()
{
    auto windows = get_windows();
    for (auto window : windows) {
        window->hide();
    }
    quit();
}

} // authppgtk