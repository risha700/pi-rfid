#include "NetworkClient.h"


NetworkClient::NetworkClient() {

    if (!network_thread_running) {
    }
    start();

    // Initialize socket, connect to server, set up callbacks, etc.
}

NetworkClient::~NetworkClient(){
    stop();
//    close(clientSocket);


}

int NetworkClient::init_socket() {

    // TODO: set it from env
    const char* iot_server_ip = "192.168.1.201";
    int iot_server_port = 8080;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(iot_server_port);
    serverAddress.sin_addr.s_addr = inet_addr(iot_server_ip); // Server's IP address


    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server: " << strerror(errno) << std::endl;
        close(clientSocket);
        return -1;
    }
    std::cout << "Connected to the server." << std::endl;

//    sleep(10);
//    test_socket();
    return 0;
}

void NetworkClient::authorize_socket() {
    // Send authentication data (username and token)
    const char* username = "user";
    std::string token = "supersecret";
    std::string authData = username + std::string(":") + token;

    send(clientSocket, authData.c_str(), authData.length(), 0);

    char buffer[2048]; // Buffer to hold received data
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
    } else {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;
        signal_data_received.emit((const std::string&)buffer);
    }
}
void NetworkClient::test_socket(){
    const char* message = "Hello, Server!";
    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent: " << message << std::endl;

        char buffer[2048]; // Buffer to hold received data
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received: " << buffer << std::endl;
            signal_data_received.emit((const std::string&)buffer);

        }
    }

}

void NetworkClient::socket_send(const char* & msg){
    const char* message = msg;
    ssize_t bytesSent = send(clientSocket, message,  strlen(message), 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent: " << message << std::endl;

        char buffer[2048]; // Buffer to hold received data
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received: " << buffer << std::endl;
            signal_data_received.emit((const std::string&)buffer);

        }
    }
}
void NetworkClient::start() {
    // Start the network thread
    if (!network_thread_running) {
        // Handle network operations here
        network_thread = std::thread([this]() {
          try
          {
            if(clientSocket == -1){
                init_socket();
//                socket_send("Hi");
//            const char*  msg="Hello programming very long message to see how many bytes will be!";
//            const char*  msg2="Yet another message 2";
//            const char*  msg3="yoo programming!!";
//            socket_send( msg);
//            std::this_thread::sleep_for(std::chrono::seconds(2));
//            socket_send( msg2);
//            std::this_thread::sleep_for(std::chrono::seconds(10));
//            socket_send( msg3);
//                socket_send("Hey C++!");
//                socket_send("yet a different message");
                network_thread_running=true;
            }

          }
          catch(const std::exception& e)
          {
            std::cerr << e.what() << '\n';
            exit(1);
          }
          
        });

    }
}

void NetworkClient::stop() {
    // Stop the network thread and clean up resources
    // Close sockets, etc.
    if (network_thread.joinable()) {
        network_thread.join();
    }
    close(clientSocket);
    
}