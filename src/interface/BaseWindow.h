#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <gtkmm.h>


class BaseWindow: public Gtk::ApplicationWindow
{
public:
    BaseWindow();


    // void open_file_view(const Glib::RefPtr<Gio::File>& file);
protected:
    //Signal handlers:
    void on_action_quit();
    void on_button_clicked();
    bool on_window_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

    Glib::RefPtr<Gio::SimpleAction> m_refChoice;
    Gtk::Box login_container;
    Gtk::Image hero_image;
    Gtk::TextView m_text;
  
};

#endif // BASEWINDOW_H