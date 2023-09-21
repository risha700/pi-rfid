//
// Created by Ahmed Shehab on 9/21/23.
//
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include "LoginWindow.h"

#include "App.h"

LoginWindow::LoginWindow():
    Gtk::ApplicationWindow(),
    login_container(Gtk::Orientation::VERTICAL, 10)
    
{
    
    set_title("RFID Admin");
    set_size_request(800, 800);
    login_container.set_margin(10);
    login_container.set_valign(Gtk::Align::BASELINE_FILL);
    hero_image.set_size_request(250,300);
    hero_image.set("assets/sec.jpg");
    login_container.prepend(hero_image);
    set_child(login_container);
    submit_btn.set_halign(Gtk::Align::CENTER);
    // username field
    username_label.set_text("Username");
    username_entry.set_name("Username");
    username_label.set_halign(Gtk::Align::START);
    password_label.set_text("Password");
    password_entry.set_name("Username");

    password_entry.set_visibility(false);
    password_label.set_halign(Gtk::Align::START);

    login_container.append(username_label);
    login_container.append(username_entry);
    login_container.append(password_label);
    login_container.append(password_entry);
    submit_btn.set_label("Submit");
    login_container.append(submit_btn);
    submit_btn.signal_clicked().connect(sigc::mem_fun(*this,&LoginWindow::on_button_clicked));
    
    


    //    add_action("quit", sigc::mem_fun(*this, &Interface::on_action_quit));
//    set_accel_for_action("app.quit", "<Ctrl>Q");

// Events.
    auto controller = Gtk::EventControllerKey::create();
    // controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);

    controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &LoginWindow::on_window_key_pressed), false);


    // controller->signal_key_pressed().connect(
    //     sigc::bind(sigc::mem_fun(*this, &LoginWindow::on_button_clicked), "capture"), false);
    
    
    add_controller(controller);

}

void LoginWindow::open_file_view(const Glib::RefPtr<Gio::File>& /* file */)
{
}

void LoginWindow::on_button_clicked()
{
    Glib::ustring usr =  username_entry.get_text();
    Glib::ustring pass =  password_entry.get_text();
    // just write it to local file for now
     if(!usr.empty() && !pass.empty()){
         bool is_auth = (usr == "op" && pass  == "op");
         if(is_auth){
            signal_login_event.emit();
             std::time_t currentTime = std::time(nullptr);
             std::fstream app_file;
             app_file.open("/tmp/rfid.sys", std::ios::out);
             if(app_file){
                 app_file << currentTime;
                 app_file.close();
             }


             // Convert the Unix timestamp to a tm structure
//             std::tm* timeInfo = std::localtime(&currentTime);

             std::cout << "Hello Interface "<< usr.c_str() << " "<< pass.c_str()  <<std::endl;

         }


     }

}

bool LoginWindow::on_window_key_pressed(guint keyval, guint, Gdk::ModifierType state)
{
    if((keyval == GDK_KEY_w) &&
       (state & (Gdk::ModifierType::CONTROL_MASK )) == Gdk::ModifierType::CONTROL_MASK)
    {
        set_visible(false);
        return true;
    }

    //the event has not been handled
    return false;
}
