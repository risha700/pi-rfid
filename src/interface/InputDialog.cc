//
// Created by Ahmed Shehab on 10/2/23.
//

#include "InputDialog.h"

InputDialog::InputDialog()
:input_label("Data to write"),
button_box(Gtk::Orientation::HORIZONTAL, 5),
send_btn("_OK", true),
cancel_btn("_Cancel", true)
{
    set_destroy_with_parent(true);
    set_title("RFID WRITE");

    set_child(m_Grid);
    m_Grid.set_row_spacing(4);
    m_Grid.set_column_spacing(4);
    m_Grid.set_expand(true);

    m_Grid.attach(input_label, 1, 0);
    m_Grid.attach(input_entry, 2, 0);
    input_label.set_mnemonic_widget(input_entry);
    m_Grid.attach(button_box, 0, 2, 3, 1);
    button_box.set_halign(Gtk::Align::END);
    button_box.append(send_btn);
    button_box.append(cancel_btn);


}
void InputDialog::buttons_clicked_connect(const sigc::slot<void(const Glib::ustring&)>& slot)
{
    send_btn.signal_clicked().connect(sigc::bind(slot, "OK"));
    cancel_btn.signal_clicked().connect(sigc::bind(slot, "Cancel"));
}

Glib::ustring InputDialog::get_entry() const
{
    return input_entry.get_text();
}
void InputDialog::show() {
    set_visible(true);
}
void InputDialog::hide() {
    set_visible(false);
}


InputDialog::~InputDialog(){};