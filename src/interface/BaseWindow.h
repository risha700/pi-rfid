#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <gtkmm.h>
#include "ControlsWindow.h"


class BaseWindow: public Gtk::ApplicationWindow
{
public:
    BaseWindow();

    void on_data_received(const std::string& data);
    
    // void open_file_view(const Glib::RefPtr<Gio::File>& file);
protected:
    //Signal handlers:
    void on_action_quit();
    void on_controls_show_click();
    void on_button_clicked();
    bool on_window_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

    Glib::RefPtr<Gio::SimpleAction> m_refChoice;
    Gtk::Box container;
    Gtk::Button m_btn,controls_btn;
    Gtk::Label message;
    Gtk::Label reader_status;
    Gtk::Entry m_entry;
    Gtk::Image hero_image;
    Gtk::TextView m_text;
    Gtk::Stack m_stack;
    ControlsWindow control_win;
};

#endif // BASEWINDOW_H