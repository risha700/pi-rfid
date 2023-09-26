//
// Created by Ahmed Shehab on 9/21/23.
//

#ifndef INTERFACE_APP_H
#define INTERFACE_APP_H

#include <gtkmm.h>
#include "LoginWindow.h"
#include "NetworkClient.h"



class App: public Gtk::Application
{
protected:
    App();
//    virtual ~App();
    
    Glib::RefPtr<Gtk::Builder> m_refBuilder;


public:
    static Glib::RefPtr<App> create();
    bool is_authenticated=false;
    LoginWindow login_window;
    NetworkClient network_client;

    void on_login_state_change();
    void on_data_received(const std::string& data);
    bool on_time_out();

    static App* get_instance();
protected:
    // Override default signal handlers:
    void on_activate() override;
    void on_open(const Gio::Application::type_vec_files& files,
                 const Glib::ustring& hint) override;
    static App* instance;

private:
    auto * create_app_window();
    auto * create_login_window();
    

};
//NetworkClient* App::network_client = nullptr;

//NetworkClient* App::network_client = nullptr;
// Declare a global pointer to your custom application instance

#endif //INTERFACE_APP_H
