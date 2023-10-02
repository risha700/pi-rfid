#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <gtkmm.h>
#include "ControlsWindow.h"
#include "AppLogger.h"
#include "App.h"


class BaseWindow: public Gtk::ApplicationWindow
{
public:
    BaseWindow();

    void on_data_received(const std::string& data);
    void on_modal_show(const std::string &data);
    // void open_file_view(const Glib::RefPtr<Gio::File>& file);
protected:
    //Signal handlers:
    void on_action_quit();
    void install_css_props();
    void on_controls_show_click();
//    void on_ping_reader_clicked();
    void on_check_network_click();
    bool on_window_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_send_command();
    App* app = App::get_instance();
    Glib::RefPtr<Gio::SimpleAction> m_refChoice;
    Gtk::Box container, status_box;
    Gtk::Button m_btn,controls_btn, send_command_btn;
    Gtk::Label connection_status,  reader_message;
    Gtk::Entry m_entry;
    Gtk::Separator m_Separator;
    Gtk::Image hero_image, status_img;
    Gtk::TextView m_text;
    Gtk::Stack m_stack;
    ControlsWindow control_win;
    InputDialog modal;
    void on_dialog_response(const Glib::ustring& response);

};

#endif // BASEWINDOW_H