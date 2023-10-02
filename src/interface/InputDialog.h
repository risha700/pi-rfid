#ifndef INTERFACE_INPUTDIALOG_H
#define INTERFACE_INPUTDIALOG_H
#include <gtkmm.h>


class InputDialog:public Gtk::Window
{
public:
    InputDialog();
    ~InputDialog() override;
    void buttons_clicked_connect(const sigc::slot<void(const Glib::ustring&)>& slot);
    Glib::ustring get_entry() const;
    void show();
    void hide();

protected:

    Gtk::Grid m_Grid;
    Gtk::Label input_label;
    Gtk::Entry input_entry;
    Gtk::Box button_box;
    Gtk::Button send_btn, cancel_btn;

};


#endif //INTERFACE_INPUTDIALOG_H
