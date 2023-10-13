#include "NetworkServer.h"
#include <systemd/sd-daemon.h>
#include <chrono>
#include <utility>

#ifndef SERVER_PORT
#define SERVER_PORT 8080
#endif

void NetworkServer::keep_listening(){

    while (true)
    {
        // accept connection
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            console_logger->error("Error accepting connection: {}",  strerror(errno));
            close(serverSocket);
            exit(1);
        }
        console_logger->info("Accepted a connection from: {}",  inet_ntoa(clientAddress.sin_addr) );

        // start receiving
        char buffer[1024]; // Buffer to hold received data
        ssize_t bytesRead=-1;


        // Read data from the client **blocking
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer),0)) > 0) {
            // Process the received data you can modify this part
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            auto req = new RFIDRequest();
            req->reader_signal = signal_card_reader; // pass the signal
            req->network_signal_data_received = signal_data_received; // pass the signal
            req->process_req_data((const std::string&)buffer);
            
        }

        if (bytesRead == -1) 
        {
            console_logger->error("Error receiving data: {}",  strerror(errno) );

            close(clientSocket);
            console_logger->info("Connection closed to: {}",  inet_ntoa(clientAddress.sin_addr) );


        }
        else if (bytesRead == 0)
        {
            close(clientSocket);
            console_logger->info("Connection closed to: {}",  inet_ntoa(clientAddress.sin_addr) );
        }

    }


}

void NetworkServer::init_socket_server(){
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
//        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        console_logger->error("Error creating socket: {}",  strerror(errno));
        exit(1);
    }
        // Set the SO_REUSEADDR option
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
//        perror("setsockopt(SO_REUSEADDR) failed");
        console_logger->error("setsockopt(SO_REUSEADDR) failed: {}",  strerror(errno));
        exit(1);
    }

//    // Set the server socket options
//    struct timeval timeout;
//    timeout.tv_sec = 10; // Set the timeout to 10 seconds
//    timeout.tv_usec = 0; // No microseconds
//
//    if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
//        console_logger->error("setsockopt(SO_RCVTIMEO) failed: {}",  strerror(errno));
//        close(serverSocket);
//        exit(1);
//    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        console_logger->error("Error binding socket: {}",  strerror(errno));

        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) == -1) {
        console_logger->error("Error listening on socket: {}",  strerror(errno));
        close(serverSocket);
        exit(1);
    }

    server_pid = getpid();

    console_logger->info("SERVER_PID {} - Server is listening on port {}...", server_pid, SERVER_PORT);

}



void NetworkServer::stop(){

    if (server_thread.joinable()) {
        job_queue_condition.notify_one();  // Unblock the thread to allow it to exit
        server_thread.join();  // Wait for the network thread to finish
    }
    server_thread_running = false;

    if(clientSocket!=-1){
        close(clientSocket);
    }

    if(serverSocket!=-1){
        close(serverSocket);
    }

    kill_process(server_pid);
    
}

void NetworkServer::start(){
   if (!server_thread.joinable()) {
        if(serverSocket == -1){
            init_socket_server();
            server_thread_running = true;
        }
        server_thread = std::thread([this]() {
            
            while (server_thread_running) {
//                keep_listening();

                std::function<void(int)> job;
                // Dequeue and execute jobs from the queue
                {
                    std::unique_lock<std::mutex> lock(job_queue_mutex);
                    job_queue_condition.wait(lock, [this] { return !server_thread_running || !job_queue.empty(); });
                    if (!server_thread_running) {
                        break;  // Exit the thread if it's no longer running
                    }
                    if (!job_queue.empty()) {
                        job = job_queue.front();
                        job_queue.pop();
                        console_logger->debug("dequeue job");
                    }

                }

                if (job) {
                    console_logger->debug("executing new job");
                    job(serverSocket);

                }
            }


        });


    }
}


void NetworkServer::run_bg(const std::function<void()> &func){
    // Add a job to the network thread's job queue
    std::unique_lock<std::mutex> lock(job_queue_mutex);

    job_queue.push([this, func](int clientSocket) {
        func();
    });
    lock.unlock();
    job_queue_condition.notify_one();


}
const void NetworkServer::heartbeat(){

    std::this_thread::sleep_for(std::chrono::seconds(10));
    sd_notify(0, ("THREADPID=" + std::to_string(getpid())).c_str());
    sd_notify(0, "WATCHDOG=1");
    console_logger->debug("❣ heartbeat ");

//    sd_notify(0, "STATUS=NOT_READY");
}

NetworkServer::NetworkServer(/* args */)
{
    console_logger =  spdlog::default_logger();

    signal_data_received.connect(sigc::mem_fun(*this, &NetworkServer::on_data_received));

    // start thread
    start();

    // Notify systemd with PID
    sd_notify(0, ("SERVICEMAINPID=" + server_pid));

    // start server
    run_bg([this]() { this->keep_listening(); });

    sd_notify(0, "READY=1");

    // start heartbeat separate thread
    auto hearbeat_thread =  std::thread([this]() {
        while (server_thread_running)
            this->heartbeat();
    });

    hearbeat_thread.detach();



}
void NetworkServer::on_data_received(const std::string &data) {
   console_logger->debug("Server Data Received... {}", data.length());
    send(clientSocket, data.c_str(), data.length(), 0);
    // authenticate(data); // go up the chain before firing TODO:
}

void NetworkServer::authenticate(const std::string &data){

    const char* buffer = data.c_str();
    
    char* username = strtok(const_cast<char*>(buffer), ":");
    char* token = strtok(NULL, ":");

    if (username != NULL && token != NULL) {
            // std::cout << "Token " << token<< "username "<<username << std::endl;
            std::cout << "Token "<<token << std::endl;
        // Check if the token matches the one generated for the user
        std::string storedToken = "supersecret"; // COMPARE IT TO THE ENCRYPTED DIGEST
        if (storedToken == token) {
            send(clientSocket, "Authentication successful\0", 26, 0);
        } else {
            send(clientSocket, "Authentication failed\0", 22, 0);
        }
    }
}
NetworkServer::~NetworkServer()
{
    stop();
    sd_notify(0, "WATCHDOG=trigger"); // force reload
}


void NetworkServer::kill_process(pid_t processId){
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
     killCommand += processName;

    // Execute the kill command
    int result = std::system(killCommand.c_str());

    if (result == 0) {
        std::cout << "Process " << processName << " killed successfully." << std::endl;
    } else {
        std::cerr << "Failed to kill process " << processName << "." << std::endl;
    }
}
