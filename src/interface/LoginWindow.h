//
// Created by Ahmed Shehab on 9/21/23.
//

#ifndef INTERFACE_LOGINWINDOW_H
#define INTERFACE_LOGINWINDOW_H
#include <gtkmm.h>

class LoginWindow : public Gtk::ApplicationWindow
{
public:
    LoginWindow();
    void open_file_view(const Glib::RefPtr<Gio::File>& file);
    using LoginSignal = sigc::signal<void()>;
    LoginSignal signal_login_event;
    Gtk::Stack* stack {nullptr};

protected:
    //Signal handlers:


    void on_action_quit();
    void on_button_clicked();
    bool on_window_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    Glib::RefPtr<Gio::SimpleAction> m_refChoice;
    Gtk::Box login_container;
    Gtk::Image hero_image;
    Gtk::Label username_label {"Username"};
    Gtk::Entry username_entry;
    Gtk::Label password_label {"Password"};
    Gtk::Entry password_entry;
    Gtk::Button submit_btn;
    Gtk::Frame label_frame;
    Gtk::Frame password_frame;
};





#endif //INTERFACE_LOGINWINDOW_H
