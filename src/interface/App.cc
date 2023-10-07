//
// Created by Ahmed Shehab on 9/21/23.
//

#include "App.h"
#include "LoginWindow.h"
#include "BaseWindow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "NetworkClient.h"


// Declare a global pointer to your custom application instance
App* App::instance = nullptr;
//AppLogger* App::my_logger = nullptr;


App::App():
Gtk::Application("rfid.interface.opaic.nz", Gio::Application::Flags::HANDLES_OPEN)
{

    Glib::set_application_name("RFID Admin");
    // set app env
 ;
    logger->info("App in {} mode",environment);

    // connect signals
    login_window.signal_login_event.connect(sigc::mem_fun(*this, &App::on_login_state_change));
    network_client.signal_data_received.connect(sigc::mem_fun(*this, &App::on_data_received));

    // Prefer dark theme
    auto settings = Gtk::Settings::get_default();
    settings->property_gtk_application_prefer_dark_theme().set_value(true);

    // test network
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &App::on_time_out),10 );
    instance = this; // set the static instance

}

// static
App* App::get_instance() {
    return instance;
}

App::~App(){}
void debug_thread(NetworkClient *network_client, int rounds=3){
    for (int i = 0; i < rounds; ++i) {
        const char*  msg="Thread test";
        int length = std::snprintf(nullptr, 0, "%s%d", msg, i);
        char* combinedString = new char[length + 1];
        std::snprintf(combinedString, length + 1, "%s%d", msg, i);
        network_client->socket_send(combinedString);
        delete[] combinedString;
    }
}
bool App::on_time_out() {
//    debug_thread(&network_client);
//    network_client.socket_send("Hi");
//    std::cout<< (std::string )environment <<std::endl;
    return false;
}

Glib::RefPtr<App> App::create()
{
    return Glib::make_refptr_for_instance<App>(new App());

}

void App::on_data_received(const std::string &data) const {
    // update UI
    logger->debug("App.cc => Data received fired... {}", data.c_str());
}

auto* App::create_login_window()
{
    auto appwindow = &this->login_window;
    
    // Make sure that the application runs for as long this window is still open.
    add_window(*appwindow);

    // A window can be added to an application with Gtk::Application::add_window()
    // or Gtk::Window::set_application(). When all added windows have been hidden
    // or removed, the application stops running (Gtk::Application::run() returns()),
    // unless Gio::Application::hold() has been called.

    // Delete the window when it is hidden.
    appwindow->signal_hide().connect([appwindow](){ delete appwindow; });

    return appwindow;
}
auto* App::create_app_window()
{
    auto appwindow = new BaseWindow();

//    appwindow->set_default_icon_name("dialog-information");
//    appwindow->set_icon_name("dialog-information");

    add_window(*appwindow);
    appwindow->signal_hide().connect([appwindow](){ delete appwindow; });
    
    return appwindow;
}
void App::on_login_state_change(){
    logger->debug("Login changed...");
    auto windows = get_windows();
    windows[0]->destroy();
    //  dynamic_cast<LoginWindow*>(windows[0]);
    auto appwindow = create_app_window();
    appwindow->present();
}

void App::on_activate()
{
    // The application has been started, so let's show a window.
    std::fstream app_file;
    app_file.open(AUTH_FS, std::ios::in);
    if(app_file){
        // quick and dirty for now
        is_authenticated = true;
    }
    app_file.close();

    // check if authenticated
    if(is_authenticated){
        // show interface menu
        auto appwindow = create_app_window();

        appwindow->present();

    }else{
        // show login form
        auto appwindow = create_login_window();
        appwindow->present();

    }


}

void App::on_open(const Gio::Application::type_vec_files& files,
                                 const Glib::ustring& /* hint */)
{



    // The application has been asked to open some files,
    // so let's open a new view for each one.
    // LoginWindow* appwindow = nullptr;
    
    // auto windows = get_windows();
    // if (windows.size() > 0)
    //     appwindow = dynamic_cast<LoginWindow*>(windows[0]);

    // if (!appwindow)
    //     appwindow = create_appwindow();

    // for (const auto& file : files)
    //     appwindow->open_file_view(file);

    // appwindow->present();
}
