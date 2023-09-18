#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class Interface : public Gtk::Window
{

public:
    Interface();
    ~Interface() override;

protected:
  //Signal handlers:
  void on_button_clicked();

  //Member widgets:
  Gtk::Button m_button;
};

#endif // INTERFACE_H