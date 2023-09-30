//
// Created by Ahmed Shehab on 9/29/23.
//

#ifndef INTERFACE_CONTROLSWINDOW_H
#define INTERFACE_CONTROLSWINDOW_H

#include <gdkmm.h>
#include <gtkmm.h>


class ControlsWindow
{
public:
    ControlsWindow();
    ~ControlsWindow();
    Gtk::Box m_VBox;

protected:
    class ModelColumns;
    // Signal handlers:
    void on_button_quit();
    void on_item_activated(unsigned int position);
    void on_selection_changed();
    void on_setup_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item);
    void on_bind_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item);
    int on_model_sort(const Glib::RefPtr<const ModelColumns>& a, const Glib::RefPtr<const ModelColumns>& b);
    void add_entry(const std::string& filename, const Glib::ustring& description);

// A Gio::ListStore stores filename, description and texture.
    class ModelColumns : public Glib::Object
    {
    public:
        std::string m_filename;
        Glib::ustring  m_description;
        Glib::RefPtr<Gdk::Texture> m_texture;

        static Glib::RefPtr<ModelColumns> create(const std::string& filename,
                                                 const Glib::ustring& description, const Glib::RefPtr<Gdk::Texture>& texture)
        {
            return Glib::make_refptr_for_instance<ModelColumns>(
                    new ModelColumns(filename, description, texture));
        }

    protected:
        ModelColumns(const std::string& filename, const Glib::ustring& description,
                     const Glib::RefPtr<Gdk::Texture>& texture)
                : m_filename(filename), m_description(description), m_texture(texture)
        { }
    }; // ModelColumns


    Glib::RefPtr<Gio::ListStore<ModelColumns>> m_data_model;
    Glib::RefPtr<Gtk::SingleSelection> m_selection_model;
    Glib::RefPtr<Gtk::SignalListItemFactory> m_factory;

    // Child widgets:
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::GridView m_GridView;
    Gtk::Box m_ButtonBox;
    Gtk::Button m_Button_Quit;


};



#endif //INTERFACE_CONTROLSWINDOW_H
