#include "NetworkServer.h"

#ifndef SERVER_PORT
#define SERVER_PORT 8080
#endif

void NetworkServer::keep_listening(){

    while (true)
    {   
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
        char buffer[1024]; // Buffer to hold received data
        ssize_t bytesRead=-1;


        // Read data from the client **blocking
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer),0)) > 0) {
            // Process the received data (you can modify this part
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            auto req = new RFIDRequest();
            req->reader_signal = signal_card_reader; // pass the signal
            req->network_signal_data_received = signal_data_received; // pass the signal
            req->process_req_data((const std::string&)buffer);
            
        }

        if (bytesRead == -1) 
        {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            close(clientSocket);
            std::cout << "Connection closed to "<< inet_ntoa(clientAddress.sin_addr)<< std::endl;  

        }
        else if (bytesRead == 0)
        {
            close(clientSocket);
            std::cout << "Connection closed to "<< inet_ntoa(clientAddress.sin_addr)<< std::endl;
        }

        
    }


}




void NetworkServer::init_socket_server(){
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(1);
    }
        // Set the SO_REUSEADDR option
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
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
                keep_listening();    
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
                    }

                }

                if (job) {
                    // Execute the job
                    job(serverSocket);

                }
            }


        });


    }
}
void NetworkServer::run_bg(const std::function<void()> &func){
    // Add a job to the network thread's job queue
    std::unique_lock<std::mutex> lock(job_queue_mutex);
//    std::lock_guard<std::mutex> lock(job_queue_mutex);
    job_queue.push([this, func](int serverSocket) {
      // add jobs here
      func();
      //signals
      //this->signal_data_received.emit((const std::string&)buffer);
    });
    lock.unlock();
    job_queue_condition.notify_one();


}
void run_echo(){
    std::cout<<"Hello echo"<<std::endl;
}
NetworkServer::NetworkServer(/* args */)
{
    signal_data_received.connect(sigc::mem_fun(*this, &NetworkServer::on_data_received));

    start();

    // run_bg(&run_echo);
    // run_bg(dynamic_cast<const std::function<void>&>(keep_listening));
}
void NetworkServer::on_data_received(const std::string &data) {
    //
    std::cout << "Server Data received fired... "<< data.c_str() << std::endl;
    // auto req = new RFIDRequest();
    // req->process_req_data(data);
    // delete req;
    // authenticate(data); // go up the chain before firing TODO: 
    ssize_t bytesSent = send(clientSocket, data.c_str(), data.length(), 0);
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
}
// decide on routes
// handle authentication


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
    // killCommand += processName;

    // Execute the kill command
    int result = std::system(killCommand.c_str());

    if (result == 0) {
        std::cout << "Process " << processName << " killed successfully." << std::endl;
    } else {
        std::cerr << "Failed to kill process " << processName << "." << std::endl;
    }
}
