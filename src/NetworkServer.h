#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include <netinet/in.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sigc++.h>
// #include <sigc++/signal.h>

class NetworkServer
{
private:
   int serverSocket=-1;
   int clientSocket=-1;
   struct sockaddr_in serverAddress;
   struct sockaddr_in clientAddress;
   std::thread network_thread;
    // using NetSignal = sigc::signal<void(const std::string&)>;
    // NetSignal signal_data_received;
   bool network_thread_running;
   void stop();
   void init_socket_server();
   void authenticate();
   pid_t server_pid;


public:
    NetworkServer();
    ~NetworkServer();
   void keep_listening();

};



#endif //NETWORKSERVER_H