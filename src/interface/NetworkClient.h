//
// Created by Ahmed Shehab on 9/22/23.
//

#ifndef INTERFACE_NETWORKCLIENT_H
#define INTERFACE_NETWORKCLIENT_H
#include <iostream>
#include <functional>
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
#include "AppLogger.h"

#ifndef RFID_SERVER_ADDR
#define RFID_SERVER_ADDR "192.168.1.201"
#endif
#ifndef RFID_SERVER_PORT
#define RFID_SERVER_PORT 8080
#endif


class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();
    void start();
    void stop();
    void run_bg(const std::function<void()> &func);
    void recv_bg_listener();
    std::shared_ptr<AppLogger> logger = AppLogger::getLogger();
    // Define signals for communication with the main GUI thread
    using NetSignal = sigc::signal<void(const std::string&)>;
    NetSignal signal_data_received;
//    using ReaderSignal = sigc::signal<void(const std::string&, const std::string&)>;

    bool test_socket();
    int clientSocket=-1;
    std::thread network_thread;
    bool network_thread_running=false;
//    void socket_send(const char* & buff);
    void socket_send(const std::string&);
    int init_socket();


private:
    void set_label_text(const std::string& text);
    struct sockaddr_in serverAddress;
    void start_bg();
    void authorize_socket();
    bool socket_has_errors=false;
//    std::queue<std::function<void(int, NetSignal, std::string)>> job_queue;
    std::queue<std::function<void(int)>> job_queue;
    std::mutex job_queue_mutex;
    std::condition_variable job_queue_condition;

};
#endif //INTERFACE_NETWORKCLIENT_H
