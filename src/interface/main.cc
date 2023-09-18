#include "interface.h"
#include <gtkmm.h>


int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("org.gtkmm.examples.base");

//   return app->make_window_and_run<Interface>(argc, argv);
    // Interface inter;
    return app->make_window_and_run<Interface>(argc, argv);
}