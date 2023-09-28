#include "BaseWindow.h"
#include "App.h"
#include <iostream>
#include <gtkmm/settings.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm.h>
//#include <hiredis/hiredis.h>


BaseWindow::BaseWindow():
    Gtk::ApplicationWindow(),
    container(Gtk::Orientation::VERTICAL, 10)
// creates a new button with label "Hello World".
{
    set_title("RFID Admin");
    set_size_request(800, 800);

    container.set_margin(10);
    container.set_valign(Gtk::Align::BASELINE_FILL);
    hero_image.set_size_request(70,70);
    hero_image.set("rfid.png");
    hero_image.set_halign(Gtk::Align::BASELINE_CENTER);
    container.prepend(hero_image);

    m_btn.set_label("check connection");
    m_btn.set_halign(Gtk::Align::START);
    m_btn.signal_clicked().connect(sigc::mem_fun(*this,&BaseWindow::on_button_clicked));
    container.append(m_btn);
    container.append(m_entry);
     //CSS style
//    Glib::ustring data = ".hero {clip-path: circle(50px at 0 100px);width: 200px;height: 200px;object-fit:fill;}";
//    auto provider = Gtk::CssProvider::create();
//    provider->load_from_data(data);
//    auto ctx = hero_image.get_style_context();
//    ctx->add_class("hero");
//    ctx->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);


    m_stack.add(container);
    message.set_text("place RFID tag");
    container.append(message);
    reader_status.set_text("Make Sure you connected MFRC522 reader!");
    container.append(reader_status);
    set_child(m_stack);


    
    
    // login_window.signal_login_event.connect(sigc::mem_fun(*this, &BaseWindow::on_login_state_change));
//    add_action("quit", sigc::mem_fun(*this, &Interface::on_action_quit));
//    set_accel_for_action("app.quit", "<Ctrl>Q");

// Events.
    auto controller = Gtk::EventControllerKey::create();

    controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &BaseWindow::on_window_key_pressed), false);


    add_controller(controller);


    auto app = App::get_instance();
    app->network_client.signal_data_received.connect(sigc::mem_fun(*this, &BaseWindow::on_data_received));


}

// Interface::~Interface()= default;

void BaseWindow::on_button_clicked()
{

    std::cout << "Hello Interface "<< std::endl;
    reader_status.set_text("checking...");

    auto app = App::get_instance();

//    app->network_client.socket_send("risha:verysecure");

    auto content = m_entry.get_text();
    std::cout << "content "<< content << std::endl;
    if(content.length()>0){
        app->network_client.socket_send(content);

    }



}
void BaseWindow::on_data_received(const std::string &data) {
    // update UI
    std::cout << "Data received fired in base window... "<< data.c_str() << std::endl;

    reader_status.set_text(data.c_str());

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


