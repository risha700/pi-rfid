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
{


    // main area
    set_title("RFID Admin");
    set_size_request(600, 600);
    set_resizable(true);
    container.set_margin(10);
    container.set_valign(Gtk::Align::BASELINE_FILL);

    hero_image.set_size_request(40,40);
    hero_image.set_from_resource("/assets/nfc.svg");
    hero_image.set_halign(Gtk::Align::START);

    container.append(hero_image);

    // status box
    status_img.set_from_resource("/assets/antenna.svg");
    status_img.set_size_request(50,50);
    status_img.set_halign(Gtk::Align::CENTER);

    connection_status.set_halign(Gtk::Align::CENTER);
    status_box.set_orientation(Gtk::Orientation::VERTICAL);
    status_box.set_spacing(10);
    send_command_btn.set_label("Send Raw Command");
    send_command_btn.set_size_request(50,50);
    status_box.set_halign(Gtk::Align::CENTER);
    status_box.append(status_img);
    status_box.append(m_Separator);
    status_box.append(connection_status);


    status_box.set_expand(false);
    container.append(status_box);
    m_btn.set_label("Check Connection");
    m_btn.set_halign(Gtk::Align::START);
    m_btn.set_size_request(25,25);

    // controls
    container.append(control_win.m_VBox);
    container.append(m_btn);
    controls_btn.set_label("RFID Controls");
    controls_btn.set_hexpand(false);
    controls_btn.set_halign(Gtk::Align::START);
    container.append(controls_btn);
    container.append(send_command_btn);
    reader_message.set_text("place RFID tag");
    container.append(m_entry);
    container.append(reader_message);
    // signals
    send_command_btn.signal_clicked().connect(sigc::mem_fun(*this, &BaseWindow::on_send_command));
    m_btn.signal_clicked().connect(sigc::mem_fun(*this,&BaseWindow::on_check_network_click));
    controls_btn.signal_clicked().connect(sigc::mem_fun(*this,&BaseWindow::on_controls_show_click));


    m_stack.add(container);
    set_child(m_stack);
    // test network once
    Glib::signal_timeout().connect([this]() {
        this->on_check_network_click();
        return false; // Returning true ensures that the timeout stops
    }, 1000);

// Events.
    auto controller = Gtk::EventControllerKey::create();

    controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &BaseWindow::on_window_key_pressed), false);


    add_controller(controller);

    app->network_client.signal_data_received.connect(sigc::mem_fun(*this, &BaseWindow::on_data_received));
    install_css_props();

    control_win.modal_signal.connect(sigc::mem_fun(*this, &BaseWindow::on_modal_show));


    // input dialog
    modal.set_default_size(300, 200);
    modal.set_transient_for(*this);
    modal.set_modal();
    modal.set_hide_on_close();
    modal.buttons_clicked_connect(
            sigc::mem_fun(*this, &BaseWindow::on_dialog_response));


}
void BaseWindow::on_modal_show(const std::string &data){
    modal.present();
}
void BaseWindow::on_dialog_response(const Glib::ustring& response)
{
    modal.set_visible(false);
    auto data = modal.get_entry();
    auto command = "RFID_WRITE:" + std::string(data);
    if (response == "OK" && !data.empty()){
        std::cout << "command: " << command <<std::endl;
        app->network_client.socket_send(command);
    }
}
void BaseWindow::on_action_quit(){
    set_visible(false);
}

void BaseWindow::on_controls_show_click(){
    control_win.m_VBox.set_visible(!control_win.m_VBox.is_visible());
}

void BaseWindow::on_send_command(){
    reader_message.set_text("...sending...\n\nPresent a tag! ");
    auto content = m_entry.get_text();
    if(!content.empty()){
        app->network_client.socket_send(content);
    }else{
        reader_message.set_text("Check your input!");
    }
}

void BaseWindow::on_check_network_click()
{
    connection_status.set_text("checking connection...");

    if(app->network_client.test_socket()){
        status_img.set_from_resource("/assets/antenna_active.svg");
        connection_status.set_text("Connected");
        connection_status.remove_css_class("connection_status_down");
        connection_status.add_css_class("connection_status_up");

    }else{
        status_img.set_from_resource("/assets/antenna.svg");
        connection_status.set_text(" Error: check your connection!");
        connection_status.remove_css_class("connection_status_up");
        connection_status.add_css_class("connection_status_down");

    }
}

void BaseWindow::on_data_received(const std::string &data) {
    // update UI
    app->logger->debug("BaseWindow=> Data received {}", data.c_str());
    reader_message.set_text(data.c_str());

}

// basic response
bool BaseWindow::on_window_key_pressed(guint keyval, guint, Gdk::ModifierType state)
{
    if((keyval == GDK_KEY_w||keyval == GDK_KEY_W||keyval == GDK_KEY_q||keyval == GDK_KEY_Q) &&
                    (
                    (state & (Gdk::ModifierType::META_MASK )) == Gdk::ModifierType::META_MASK
                    ||(state & (Gdk::ModifierType::CONTROL_MASK )) == Gdk::ModifierType::CONTROL_MASK)
                    )
    {
        set_visible(false);
        return true;
    }
    return false;
}



void BaseWindow::install_css_props() {
    // Concerning color in CSS files, see https://www.w3.org/TR/css-color-3.
    const std::string css = "#connection_status { padding:10px; border-radius:10px;}"
                            ".connection_status_up {background-color: #50C878;}"
                            ".connection_status_down {background-color: #FF8269;}";

    connection_status.set_name("connection_status");
    auto css_provider = Gtk::CssProvider::create();
    Gtk::StyleProvider::add_provider_for_display(
            connection_status.get_display(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    css_provider->load_from_data(css);
//    css_provider->signal_parsing_error().connect(
//            sigc::mem_fun(*this, &ExampleWindow::on_parsing_error));
}