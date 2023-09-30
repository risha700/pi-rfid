#include "App.h"


int main(int argc, char* argv[])
{
//  auto application = Gtk::Application::create("org.gtkmm.examples.base.nz");
//
//   return application->make_window_and_run<ControlsWindow>(argc, argv);
    // Interface inter;
//    return app->make_window_and_run<Interface>(argc, argv);

    auto application =  App::create();
////    ControlsWindow win;

    return application->run(argc, argv);
}