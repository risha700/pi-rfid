#include "NetworkClient.h"
#include "AppLogger.h"


//auto logger = spdlog::default_logger();

NetworkClient::NetworkClient() {
    // Initialize socket, connect to server, set up callbacks, etc.
    if (!network_thread_running) {
        start();
    }

}

NetworkClient::~NetworkClient(){
    stop();
//    close(clientSocket);


}
void NetworkClient::recv_bg_listener(){
    char buffer[2048]; // Buffer to hold received data
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        logger->error("Error receiving data:  {}",strerror(errno));

    } else {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        logger->debug("Received:  {}",buffer);
        this->signal_data_received.emit((const std::string&)buffer);

    }
}

void NetworkClient::run_bg(const std::function<void()> &func){
    // Add a job to the network thread's job queue
    std::unique_lock<std::mutex> lock(job_queue_mutex);
//    std::lock_guard<std::mutex> lock(job_queue_mutex);
    job_queue.push([func](int clientSocket) {
        // add jobs here
        func();
        //signals
        //this->signal_data_received.emit((const std::string&)buffer);
    });
    lock.unlock();
    job_queue_condition.notify_one();


}


int NetworkClient::init_socket() {


    // TODO: set it from env
    const char* iot_server_ip = RFID_SERVER_ADDR;
    int iot_server_port = RFID_SERVER_PORT;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        logger->error("Error creating socket:  {}",strerror(errno));
        return -1;
    }

    // Set timeout for receiving
    struct timeval timeout;
    timeout.tv_sec = 10;  // 10 seconds
    timeout.tv_usec = 0;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        logger->error("Error setting receiving timeout:  {}",strerror(errno));
        close(clientSocket);
        return -1;
    }

    // Set timeout for sending
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        logger->error("Error setting send timeout:  {}",strerror(errno));
        close(clientSocket);
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(iot_server_port);
    serverAddress.sin_addr.s_addr = inet_addr(iot_server_ip); // Server's IP address


    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        logger->error("Error connecting to server:  {}",strerror(errno));
        close(clientSocket);
        return -1;
    }
    logger->info("Connected to the server {}:{}", iot_server_ip, iot_server_port);

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
        logger->error("Error receiving data:  {}",strerror(errno));
    } else {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        logger->debug("Received:  {}",buffer);

        signal_data_received.emit((const std::string&)buffer);
    }
}

bool NetworkClient::test_socket(){
//    if(clientSocket == -1){
        close(clientSocket);
        auto test_server = init_socket();
        return test_server == 0;
//    }
//    return true;
}

void NetworkClient::socket_send(const std::string &message){
    // Add a job to the network thread's job queue
    std::unique_lock<std::mutex> lock(job_queue_mutex);
    job_queue.push([this, message](int clientSocket) {
        ssize_t bytesSent = send(clientSocket, message.c_str(),  message.length(), 0);
        if (bytesSent == -1) {
            logger->error("Error sending data:  {}", strerror(errno));
        }else if(bytesSent==0){ // socket closed actually
             exit(1);
        } else {
            logger->debug("Sent:  {}", message);
            recv_bg_listener();
        }
    });

    lock.unlock();
    job_queue_condition.notify_one();


}


void NetworkClient::start() {
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

