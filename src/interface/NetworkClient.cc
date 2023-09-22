#include "NetworkClient.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

NetworkClient::NetworkClient() {
    network_thread_running = false;

    // Initialize socket, connect to server, set up callbacks, etc.
}

NetworkClient::~NetworkClient(){
    network_thread.join();
    //close(socket)


}

void NetworkClient::start() {
    // Start the network thread
    if (!network_thread_running) {

//        network_thread = std::thread([this]() {
//
//            // Handle network operations here
//        });

    }
}

void NetworkClient::stop() {
    // Stop the network thread and clean up resources
    // Close sockets, etc.
    if (network_thread.joinable()) {
        network_thread.join();
    }
}