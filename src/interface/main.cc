#include "App.h"

int main(int argc, char* argv[])
{
//  auto application = Gtk::Application::create("org.gtkmm.examples.base");

//   return app->make_window_and_run<Interface>(argc, argv);
    // Interface inter;
//    return app->make_window_and_run<Interface>(argc, argv);

    auto application =  App::create();


    return application->run(argc, argv);
}