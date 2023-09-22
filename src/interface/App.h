//
// Created by Ahmed Shehab on 9/21/23.
//

#ifndef INTERFACE_APP_H
#define INTERFACE_APP_H

#include <gtkmm.h>
#include "LoginWindow.h"

class App: public Gtk::Application
{
protected:
    App();
    
    Glib::RefPtr<Gtk::Builder> m_refBuilder;


public:
    static Glib::RefPtr<App> create();
    bool is_authenticated;
    LoginWindow login_window;
    void on_login_state_change();

protected:
    // Override default signal handlers:
    void on_activate() override;
    void on_open(const Gio::Application::type_vec_files& files,
                 const Glib::ustring& hint) override;
                 

private:
    auto * create_app_window();
    auto * create_login_window();
    

};



#endif //INTERFACE_APP_H
