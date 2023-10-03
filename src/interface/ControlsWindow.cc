#include "ControlsWindow.h"
#include "App.h"
#include "BaseWindow.h"
#include <array>
#include <iostream>


namespace
{
    struct GridEntry
    {
        std::string m_filename;
        Glib::ustring m_description;
    };

    std::array<GridEntry, 4> entries =
            {
                GridEntry{"/assets/read.svg", "RFID_PING"},
                GridEntry{"/assets/scan.svg", "RFID_READ"},
                GridEntry{"/assets/present_tag.png", "RFID_DUMP"},
                GridEntry{"/assets/write.svg", "RFID_WRITE"}
            };

} // anonymous namespace

// not a window instance just a name
ControlsWindow::ControlsWindow()
:m_VBox(Gtk::Orientation::VERTICAL), m_Button_Quit("Dismiss")
{

//    set_title("Grid example");
//    set_default_size(600, 500);
    m_VBox.set_margin(5);

//    set_child(m_VBox);
    // Add the GridView inside a ScrolledWindow, with the button underneath:
    m_ScrolledWindow.set_child(m_GridView);

    // Only show the scrollbars when they are necessary:
    m_ScrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_ScrolledWindow.set_expand(false);
    m_ScrolledWindow.set_min_content_height(100);

    m_VBox.append(m_ScrolledWindow);
    m_VBox.append(m_ButtonBox);

    m_ButtonBox.append(m_Button_Quit);
    m_ButtonBox.set_margin(6);
    m_Button_Quit.set_hexpand(true);
    m_Button_Quit.set_halign(Gtk::Align::END);
    m_Button_Quit.signal_clicked().connect(
            sigc::mem_fun(*this, &ControlsWindow::on_button_quit));

    // Create the data model:
    m_data_model = Gio::ListStore<ModelColumns>::create();
    m_selection_model = Gtk::SingleSelection::create(m_data_model);
    m_selection_model->set_autoselect(false);

    m_factory = Gtk::SignalListItemFactory::create();
    m_factory->signal_setup().connect(
            sigc::mem_fun(*this, &ControlsWindow::on_setup_listitem));
    m_factory->signal_bind().connect(
            sigc::mem_fun(*this, &ControlsWindow::on_bind_listitem));

    // Fill the Gio::ListStore's data model and sort it.
    for (const auto& entry : entries)
        add_entry(entry.m_filename, entry.m_description);
//    m_data_model->sort(sigc::mem_fun(*this, &ControlsWindow::on_model_sort));

    m_GridView.set_model(m_selection_model);
    m_GridView.set_factory(m_factory);

    m_GridView.signal_activate().connect(
            sigc::mem_fun(*this, &ControlsWindow::on_item_activated));
    m_selection_model->property_selected().signal_changed().connect(
            sigc::mem_fun(*this, &ControlsWindow::on_selection_changed));


}


void ControlsWindow::on_button_quit()
{
    m_VBox.set_visible(false);
}

void ControlsWindow::on_setup_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    // Each ListItem contains a vertical Box with an Image and a Label.
    auto vBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    auto picture = Gtk::make_managed<Gtk::Picture>();
    picture->set_can_shrink(true);
    picture->set_halign(Gtk::Align::CENTER);
    picture->set_valign(Gtk::Align::END);
    picture->set_size_request(50,50);
    vBox->append(*picture);
    vBox->append(*Gtk::make_managed<Gtk::Label>());
    list_item->set_child(*vBox);
}

void ControlsWindow::on_bind_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto col = std::dynamic_pointer_cast<ModelColumns>(list_item->get_item());
    if (!col)
        return;
    auto vBox = dynamic_cast<Gtk::Box*>(list_item->get_child());
    if (!vBox)
        return;
    auto picture = dynamic_cast<Gtk::Picture*>(vBox->get_first_child());
    if (!picture)
        return;
    auto label = dynamic_cast<Gtk::Label*>(picture->get_next_sibling());
    if (!label)
        return;
    picture->set_paintable(col->m_texture);
    label->set_markup(col->m_description);
}

void ControlsWindow::on_item_activated(unsigned int position)
{
    auto col = m_data_model->get_item(position);
    if (!col)
        return;

    const std::string filename = col->m_filename;
    const Glib::ustring description = col->m_description;

    auto app = App::get_instance();
    app->logger->debug("Item activated: filename={}, description={}", filename.c_str(), description.c_str());
    // if the action is RFID_WRITE
    if(strcmp(description.c_str(), "RFID_WRITE") == 0){
        app->logger->debug("looking for input to write");
        modal_signal.emit("present");
    }else{
        app->network_client.socket_send(description.c_str());
    }
}


ControlsWindow::~ControlsWindow() {};

void ControlsWindow::on_selection_changed()
{
    auto position = m_selection_model->get_selected();
    auto col = m_data_model->get_item(position);
    if (!col)
        return;

    const std::string filename = col->m_filename;
    const Glib::ustring description = col->m_description;

//    std::cout  << "Selection Changed to: filename=" << filename
//               << ", description=" << description << std::endl;
}

int ControlsWindow::on_model_sort(const Glib::RefPtr<const ModelColumns>& a,
                                 const Glib::RefPtr<const ModelColumns>& b)
{
    return (a->m_description < b->m_description) ? -1 : ((a->m_description > b->m_description) ? 1 : 0);

}
//
void ControlsWindow::add_entry(const std::string& filename,
                              const Glib::ustring& description )
{
    try
    {
//        auto pixbuf = Gdk::Pixbuf::create_from_file(filename);
        auto pixbuf = Gdk::Pixbuf::create_from_resource(filename);
        auto texture = Gdk::Texture::create_for_pixbuf(pixbuf);
        m_data_model->append(ModelColumns::create(filename, description, texture));
    }
    catch (const Gdk::PixbufError& ex)
    {
        std::cerr << "Gdk::PixbufError: " << ex.what() << std::endl;
    }
    catch (const Glib::FileError& ex)
    {
        std::cerr << "Glib::FileError: " << ex.what() << std::endl;
    }

}
