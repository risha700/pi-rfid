#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
void kill_process(pid_t processId){
    const char* processName = "your_process_name_or_id_here";

    // Build the kill command based on the operating system
    // For Unix-like systems (Linux, macOS)
    // Build the kill command based on the operating system
    #if defined(__unix__) || defined(__APPLE__) // For Unix-like systems (Linux, macOS)
        std::string killCommand = "kill -9 " + std::to_string(processId);
    #elif defined(_WIN32) // For Windows
        std::string killCommand = "taskkill /F /PID " + std::to_string(processId);
    #else
        #error Unsupported operating system
    #endif
    // Append the process name or ID to the command
    // killCommand += processName;

    // Execute the kill command
    int result = std::system(killCommand.c_str());

    if (result == 0) {
        std::cout << "Process " << processName << " killed successfully." << std::endl;
    } else {
        std::cerr << "Failed to kill process " << processName << "." << std::endl;
    }
}

bool auth_check(int clientSocket, char* buffer){
    // Split received data into username and token

    char* username = strtok(buffer, ":");
    char* token = strtok(NULL, ":");

    if (username != NULL && token != NULL) {
            // std::cout << "Token " << token<< "username "<<username << std::endl;
            // std::cout << "Token "<<token << std::endl;
        // Check if the token matches the one generated for the user
        std::string storedToken = "supersecret"; // COMPARE IT TO THE ENCRYPTED DIGEST
        if (storedToken == token) {
            send(clientSocket, "Authentication successful\0", 26, 0);
            return true;
        } else {
            send(clientSocket, "Authentication failed\0", 22, 0);
            return false;
        }
    }
    return false;
}

int main() {

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    int clientSocket;
    pid_t pid;
    bool isAuthorized=true;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);
    // Set the SO_REUSEADDR option
    // int reuse = 1;
    // if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    //     perror("setsockopt(SO_REUSEADDR) failed");
    //     exit(1);
    // }
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }
    // int is_bound = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        // start blocking server
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }
    
    pid = getpid();

    printf("pid: %lu\t", pid);
    std::cout << "Server is listening on port 8080..." << std::endl;

    while(true){
        // accept connection
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            close(serverSocket);
            exit(1);
        }
        std::cout << "Accepted a connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;
        bool session_on=true;

        // start receiving
        char buffer[2048]; // Buffer to hold received data
        ssize_t bytesRead=-1;

        // Read data from the client
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer),0)) > 0) {
            // Process the received data (you can modify this part
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received " << bytesRead << " bytes from client: " << buffer << std::endl;
            ssize_t bytesSent = send(clientSocket, buffer, bytesRead, 0);
        }

        if (bytesRead == -1) 
        {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        }
        if (bytesRead == 0)
        {
            close(clientSocket);
            std::cout << "Connection closed to "<< inet_ntoa(clientAddress.sin_addr)<< std::endl;                
        }


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
    close(clientSocket);
    close(serverSocket);
    std::cout << "Connection closed to "<< inet_ntoa(clientAddress.sin_addr)<< std::endl;

    kill_process(pid);
    sleep(1000);
    return 0;
}
// 