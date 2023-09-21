#include "BaseWindow.h"
#include <iostream>
#include <gtkmm/settings.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm.h>
//#include <hiredis/hiredis.h>


BaseWindow::BaseWindow():
    Gtk::ApplicationWindow(),
    login_container(Gtk::Orientation::VERTICAL, 10)
// creates a new button with label "Hello World".
{
    set_title("RFID Admin");
    set_size_request(800, 800);
    login_container.set_margin(10);
    login_container.set_valign(Gtk::Align::BASELINE_FILL);
    hero_image.set_size_request(50,50);
    hero_image.set("rfid.png");
    
    login_container.prepend(hero_image);
    
     //CSS style
    Glib::ustring data = ".window {clip-path: circle(50px at 0 100px);width: 200px;height: 200px;}";
    auto provider = Gtk::CssProvider::create();
    provider->load_from_data(data);
    auto ctx = hero_image.get_style_context();
    ctx->add_class("window");
    ctx->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    set_child(login_container);
    
    
    // login_window.signal_login_event.connect(sigc::mem_fun(*this, &BaseWindow::on_login_state_change));
//    add_action("quit", sigc::mem_fun(*this, &Interface::on_action_quit));
//    set_accel_for_action("app.quit", "<Ctrl>Q");

// Events.
    auto controller = Gtk::EventControllerKey::create();

    controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &BaseWindow::on_window_key_pressed), false);
    add_controller(controller);
}

// Interface::~Interface()= default;

void BaseWindow::on_button_clicked()
{

  std::cout << "Hello Interface "<< std::endl;

}

bool BaseWindow::on_window_key_pressed(guint keyval, guint, Gdk::ModifierType state)
{

//    std::cout << "Key pressed "<< keyval << " state " << static_cast<int >(state) << std::endl;

    if((keyval == GDK_KEY_w) &&
            (state & (Gdk::ModifierType::CONTROL_MASK )) == Gdk::ModifierType::CONTROL_MASK)
    {
        set_visible(false);
        return true;
    }

    //the event has not been handled
    return false;
}


