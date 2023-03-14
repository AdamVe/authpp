#include "main_window.h"

#include <gtkmm/application.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.adamve.authppgtk");
    return app->make_window_and_run<authppgtk::MainWindow>(argc, argv);
}
