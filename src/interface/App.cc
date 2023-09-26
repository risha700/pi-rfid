//
// Created by Ahmed Shehab on 9/21/23.
//

#include "App.h"
#include "LoginWindow.h"
#include "BaseWindow.h"
#include <iostream>
#include <fstream>
#include <string>
#include "NetworkClient.h"

#ifndef AUTH_FS
#define AUTH_FS "/tmp/rfid.sys"
#endif
App* App::instance = nullptr;

App::App():
Gtk::Application("rfid.opaic.assignment.sep.nz", Gio::Application::Flags::HANDLES_OPEN) {
    // connect signals
    login_window.signal_login_event.connect(sigc::mem_fun(*this, &App::on_login_state_change));
    network_client.signal_data_received.connect(sigc::mem_fun(*this, &App::on_data_received));

    // test network
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &App::on_time_out),10 );

    instance = this; // set the static instance

}

// static
App* App::get_instance() {
    return instance;
}

//App::~App(){}
void debug_thread(NetworkClient *network_client, int rounds=3){
    for (int i = 0; i < rounds; ++i) {
        const char*  msg="yoo programming ";
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
    return false;
}

Glib::RefPtr<App> App::create()
{
    return Glib::make_refptr_for_instance<App>(new App());

}

void App::on_data_received(const std::string &data) {
    // update UI
    std::cout << "Data received fired... "<< data.c_str() << std::endl;



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
    add_window(*appwindow);
    appwindow->signal_hide().connect([appwindow](){ delete appwindow; });
    
    return appwindow;
}
void App::on_login_state_change(){
    std::cout << "Login changed..."<< std::endl;
    
    auto windows = get_windows();
    windows[0]->destroy();
//    dynamic_cast<LoginWindow*>(windows[0])
//    this->remove_window();
    //  dynamic_cast<LoginWindow*>(windows[0]);
    
    auto appwindow = create_app_window();
    appwindow->present();
     

    // is_authenticated = true;
//    set_auth((bool)is_auth);

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
