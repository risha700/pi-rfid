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


class NetworkServer
{
private:
   int serverSocket=-1;
   int clientSocket=-1;
   struct sockaddr_in serverAddress;
   struct sockaddr_in clientAddress;
   std::thread network_thread;
   bool network_thread_running;
   void start();
   void stop();
   void init_socket_server();


public:
    NetworkServer(/* args */);
    ~NetworkServer();

};


void NetworkServer::start(){


}
void NetworkServer::init_socket_server(){

}

void NetworkServer::stop(){
    
    if (network_thread.joinable()) {
        network_thread.join();
    }
    // close socket
    
}

NetworkServer::NetworkServer(/* args */)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }

    pid_t pid = getpid();

    printf("pid: %lu\t", pid);
    std::cout << "Server is listening on port 8080..." << std::endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }

    std::cout << "Accepted a connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

    char buffer[1024]; // Buffer to hold received data
    
        // Receive data from the client
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
    } else {
        
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        // if(auth_check(clientSocket, buffer)){
            std::cout << "Received: " << buffer << std::endl;

            // Echo the received data back to the client
            ssize_t bytesSent = send(clientSocket, buffer, bytesRead, 0);
            if (bytesSent == -1) {
                std::cerr << "Error sending data: " << strerror(errno) << std::endl;
            }
        // }

    }

    
    // // Split received data into username and token

    // char* username = strtok(buffer, ":");
    // char* token = strtok(NULL, ":");

    // if (username != NULL && token != NULL) {
    //         // std::cout << "Token " << token<< "username "<<username << std::endl;
    //         std::cout << "Token "<<token << std::endl;
    //     // Check if the token matches the one generated for the user
    //     std::string storedToken = "supersecret"; // COMPARE IT TO THE ENCRYPTED DIGEST
    //     if (storedToken == token) {
    //         send(clientSocket, "Authentication successful\0", 26, 0);
    //     } else {
    //         send(clientSocket, "Authentication failed\0", 22, 0);
    //     }
    // }



    // Close the sockets
    // close(clientSocket);
    // close(serverSocket);
    // kill_process(pid);
}

NetworkServer::~NetworkServer()
{
    stop();
}


#endif //NETWORKSERVER_H