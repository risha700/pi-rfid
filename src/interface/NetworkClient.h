//
// Created by Ahmed Shehab on 9/22/23.
//

#ifndef INTERFACE_NETWORKCLIENT_H
#define INTERFACE_NETWORKCLIENT_H
#include <gtkmm.h>
#include <thread>

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();
    void start();
    void stop();
    // Define signals for communication with the main GUI thread
//    sigc::signal<void, const std::string&> signal_data_received;
    using NetSignal = sigc::signal<void(), const std::string&>;
//    NetSignal signal_data_received;

private:
    std::thread network_thread;
    bool network_thread_running;
    void set_label_text(const std::string& text);
    // Other members for socket, buffers, etc.
};
#endif //INTERFACE_NETWORKCLIENT_H
