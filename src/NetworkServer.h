#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include <iostream>
#include <thread>
#include <functional>
#include <stdio.h>
#include <cstdlib>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <map>
#include <sigc++.h>
#include <spdlog/spdlog.h>


using ReaderSignal = sigc::signal<void(const std::string&, const std::string&)>;
using NetSignal = sigc::signal<void(const std::string&)>;


enum class RequestType
{
    RFID_PING = 0,
    RFID_AUTH = 100,
    RFID_READ = 200,
    RFID_WRITE = 300,
    RFID_DUMP = 400,
};


//
// we allowed yet 2048 bytes/ 2mb buffer for each request request structure 
// AUTH:username:token
// READ:
// WRITE: string value
// DUMP: 


class NetworkServer
{
private:
   int serverSocket=-1;
   int clientSocket=-1;
   struct sockaddr_in serverAddress;
   struct sockaddr_in clientAddress;
   std::thread server_thread;
   void stop();
   void start();
   void init_socket_server();
   void authenticate(const std::string &data);
   void kill_process(pid_t processId);
   void on_data_received(const std::string &data);
   pid_t server_pid;

   const void heartbeat();
    // Initialize the logger with a console sink
   std::shared_ptr<spdlog::logger> console_logger;
   std::queue<std::function<void(int)>> job_queue;
   std::mutex job_queue_mutex;
   std::condition_variable job_queue_condition;
   
public:
    NetworkServer();
    ~NetworkServer();
    void keep_listening();
    bool server_thread_running=false;
    NetSignal signal_data_received;
    ReaderSignal signal_card_reader;

//    template <typename FuncType, typename... Args>
//    void run_bg(const FuncType &&func, Args&&... args);
    void run_bg(const std::function<void()> &func);

};




struct RFIDRequest {
    RequestType type;
    // std::map<std::string, std::function<void()>> type;
    std::map<std::string, RequestType> ALLOWED_REQUESTS = {
        {"RFID_PING", RequestType::RFID_PING},
        {"RFID_AUTH", RequestType::RFID_AUTH},
        {"RFID_READ", RequestType::RFID_READ},
        {"RFID_WRITE", RequestType::RFID_WRITE},
        {"RFID_DUMP", RequestType::RFID_DUMP}
    };

    std::string data="";
    std::string response="ERROR_GENERIC";
    
    ReaderSignal reader_signal;
    NetSignal network_signal_data_received;
    std::string command = "**signal reader**";

    std::function<void()> fn;
    void process_req_data(const std::string &raw_data){
        std::istringstream iss(raw_data);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(iss, token, ':')) {
            tokens.push_back(token);
        }
        
        auto req_type = tokens[0]; // must be first argument

        if(ALLOWED_REQUESTS.find(req_type) != ALLOWED_REQUESTS.end()){ // if we know the request
            type =  ALLOWED_REQUESTS[req_type];

            switch (type)
                {
                case RequestType::RFID_PING:
                    response = "RFID_PONG";
                    network_signal_data_received.emit((const std::string&)response);
//                    std::cout << "GOT RFID_PING"<< std::endl;
                    break;
                case RequestType::RFID_AUTH:
                    // TODO: check auth and assign response
                     response = "PROCESSING";
                     command = "RFID_AUTH";
                     reader_signal.emit((const std::string&)command, (const std::string&)data);
//                    std::cout << "RFID_AUTH" << std::endl;
                    break;
                case RequestType::RFID_READ:
                    // check if auth
                    // check if tag in place
                    // scan and send back results form card reader
                     // const char* note = "**emitted**";
                     // signal.emit(const_cast<std::string&>(note));
                     response = "PROCESSING";
                     command = "RFID_READ";
                     reader_signal.emit((const std::string&)command, (const std::string&)data);
//                     std::cout << "RFID_READ"<< std::endl;
                    break;
                case RequestType::RFID_WRITE:
                    if(tokens.size() >=2){
                        data = tokens[1];
                        // response=data; // todo send signal to reader
                    }
                    response = "PROCESSING";
                    command = "RFID_WRITE";
                    reader_signal.emit((const std::string&)command, (const std::string&)data);
//                    std::cout << "RFID_WRITE-"<<"DATA:\t"<<data.c_str()<< std::endl;
                    break; 
                case RequestType::RFID_DUMP:
                    command = "RFID_DUMP";
                    reader_signal.emit((const std::string&)command, (const std::string&)data);
//                    std::cout << "RFID_DUMP "<< std::endl;
                    break; 
                default:
                    response = "ERROR: Unknown Request";
                    network_signal_data_received.emit((const std::string&)response);
//                    std::cout << "Error: Unknown request type "<< (int)type << std::endl;
                    break;
                }
                
                // for (const auto& t : tokens) {
                //     std::cout << "token is "<<t << std::endl;
                // }

        }else{
            response = "ERROR: Unknown Request";
            network_signal_data_received.emit((const std::string&)response.c_str());
            // reject request
            // throw exception
        }

      
        // const char* buffer = raw_data.c_str();
        // char* username = strtok(const_cast<char*>(buffer), ":");
        // char* token = strtok(NULL, ":");

    }
    

};// end RFIDRequest




#endif //NETWORKSERVER_H