//
// Created by Ahmed Shehab on 9/22/23.
//

#ifndef INTERFACE_NETWORKCLIENT_H
#define INTERFACE_NETWORKCLIENT_H
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <gtkmm.h>
#include <thread>
#include <netinet/in.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <sys/socket.h>
#include <arpa/inet.h>


class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();
    void start();
    void stop();
    // Define signals for communication with the main GUI thread
    using NetSignal = sigc::signal<void(const std::string&)>;
    NetSignal signal_data_received;
    void test_socket();
    int clientSocket=-1;
    std::thread network_thread;
    bool network_thread_running= false;
    void socket_send(const char* & buff);


private:
    void set_label_text(const std::string& text);
    struct sockaddr_in serverAddress{};
    int init_socket();
    void authorize_socket();
    std::queue<std::function<void(int)>> job_queue;
    std::mutex job_queue_mutex;
    std::condition_variable job_queue_condition;
    // Other members for socket, buffers, etc.
};
#endif //INTERFACE_NETWORKCLIENT_H
