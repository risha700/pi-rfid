#include "NetworkClient.h"


NetworkClient::NetworkClient() {
    // Initialize socket, connect to server, set up callbacks, etc.
    if (!network_thread_running) {
        start_bg();
    }

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

void NetworkClient::socket_send(const std::string &message){
    // Add a job to the network thread's job queue
    std::unique_lock<std::mutex> lock(job_queue_mutex);
//    std::lock_guard<std::mutex> lock(job_queue_mutex);
    job_queue.push([this, message](int clientSocket) {

//        const char* message = msg;
        ssize_t bytesSent = send(clientSocket, message.c_str(),  message.length(), 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending data: " << strerror(errno) << std::endl;
        }else if(bytesSent==0){
             exit(1);
        } else {
            std::cout << "Sent: " << message << std::endl;

            char buffer[2048]; // Buffer to hold received data
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead == -1) {
                std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            } else {
                buffer[bytesRead] = '\0'; // Null-terminate the received data
                std::cout << "Received: " << buffer << std::endl;
                this->signal_data_received.emit((const std::string&)buffer);

            }
        }
    });
    lock.unlock();
    job_queue_condition.notify_one();


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

void NetworkClient::start_bg() {
    if (!network_thread.joinable()) {
        if(clientSocket == -1){
            init_socket();
            network_thread_running = true;
        }
        network_thread = std::thread([this]() {
            while (network_thread_running) {
                std::function<void(int)> job;
                // Dequeue and execute jobs from the queue
                {
                    std::unique_lock<std::mutex> lock(job_queue_mutex);
                    job_queue_condition.wait(lock, [this] { return !network_thread_running || !job_queue.empty(); });
                    if (!network_thread_running) {
                        break;  // Exit the thread if it's no longer running
                    }
                    if (!job_queue.empty()) {
                        job = job_queue.front();
                        job_queue.pop();
                    }

                }

                if (job) {
                    // Execute the job
                    job(clientSocket);

                }
            }


        });


    }
}

void NetworkClient::stop() {

    network_thread_running = false;

    if (network_thread.joinable()) {
        job_queue_condition.notify_one();  // Unblock the thread to allow it to exit
        network_thread.join();  // Wait for the network thread to finish
    }
    if(clientSocket!=-1){
        close(clientSocket);
    }

}