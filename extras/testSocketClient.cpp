#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main() {
    // TODO: set it from env
    const char* iot_server_ip = "192.168.1.201";  
    int iot_server_port = 12345;              

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(iot_server_port);
    serverAddress.sin_addr.s_addr = inet_addr(iot_server_ip); // Server's IP address
    // serverAddress.sin_addr.s_addr = inet_addr("192.168.1.201"); // Server's IP address

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }

    std::cout << "Connected to the server." << std::endl;

    const char* message = "Hello, Server!";
    
    // Send authentication data (username and token)
    const char* username = "user";
    std::string token = "supersecret";
    std::string authData = username + std::string(":") + token;
    send(clientSocket, authData.c_str(), authData.length(), 0);

    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);

    if (bytesSent == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent: " << message << std::endl;

        char buffer[1024]; // Buffer to hold received data
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received: " << buffer << std::endl;
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}