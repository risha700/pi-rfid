#include "NetworkServer.h"

#ifndef SERVER_PORT
#define SERVER_PORT 8080
#endif

void NetworkServer::keep_listening(){

    // init_socket_server();
    // accept connection
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }
    std::cout << "Accepted a connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;
    

    // start receiving
    char buffer[2048]; // Buffer to hold received data
    ssize_t bytesRead=-1;

    // Read data from the client **blocking
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






}
void NetworkServer::init_socket_server(){
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

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

    server_pid = getpid();

    printf("pid: %d\t", server_pid);
    std::cout << "Server is listening on port 8080..." << std::endl;

}

void NetworkServer::stop(){
    
    if (network_thread.joinable()) {
        network_thread.join();
    }
    // close socket
    // Close the sockets
    close(clientSocket);
    close(serverSocket);
    // kill_process(pid);
    
}

NetworkServer::NetworkServer(/* args */)
{
    init_socket_server();
}

NetworkServer::~NetworkServer()
{
    stop();
}
// decide on routes
// handle authentication


// int main() {
//     NetworkServer server;

//     while (true)
//     {
//         server.keep_listening();
//         /* code */
//     }
    
//     return 0;
// }