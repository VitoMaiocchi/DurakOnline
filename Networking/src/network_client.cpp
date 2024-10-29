#include <Networking/network.hpp>
#include <sockpp/tcp_connector.h>
#include <sockpp/tcp_acceptor.h>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>
#include <optional>
#include <list>
#include <queue>
#include <utility>
#include <chrono>
#include <map>
#include <unordered_set>

#define BUFFER_SIZE 2048
#define MESSAGE_TIME 100

namespace Network {
    //CLIENT USE
    sockpp::tcp_connector send_connector;
    sockpp::tcp_connector recive_connector;
    ClientID client_id;
    std::mutex message_queue_mut;
    std::queue<std::string> message_queue;
    std::thread recive_thread;

    ClientID openConnection(std::string ip, uint port) {
        std::cout << "opening connection..." << std::endl;
        if(!send_connector.connect(sockpp::inet_address(ip, port)))
            throw std::runtime_error("FAILED TO OPEN CONNECTION");
        send_connector.send("request id");
        char buffer[BUFFER_SIZE];
        size_t n = 0;
        while (n == 0) n = send_connector.recv(buffer, sizeof(buffer)).value_or_throw();
        client_id = std::stoi(std::string(buffer, n));
        std::cout << "client id ["<< client_id<<"]" << std::endl;
        if(!recive_connector.connect(sockpp::inet_address(ip, port)))
            throw std::runtime_error("FAILED TO OPEN CONNECTION");
        recive_connector.send("recive " + std::to_string(client_id));
        recive_thread = std::thread([](){
            char buffer[BUFFER_SIZE];
            while(true) {
                size_t n = 0;
                while (n == 0) n = recive_connector.recv(buffer, sizeof(buffer)).value_or_throw();
                message_queue_mut.lock();
                message_queue.push(std::string(buffer,n));
                message_queue_mut.unlock();
            }
        });
        std::cout << "connection established" << std::endl;
        return client_id;
    }


    std::chrono::system_clock::time_point last_message;
    void sendMessage(std::unique_ptr<Message> &message) {
        auto now = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_message).count();
        if(delta < MESSAGE_TIME) std::this_thread::sleep_for(std::chrono::milliseconds(MESSAGE_TIME - delta));
        last_message = std::chrono::system_clock::now();

        send_connector.send(message->toJson());
    }

    std::unique_ptr<Message> reciveMessage() {
        message_queue_mut.lock();
        if(!message_queue.empty()) {
            auto m = message_queue.front();
            message_queue.pop();
            message_queue_mut.unlock();
            return deserialiseMessage(m);
        }
        message_queue_mut.unlock();
        return nullptr;
    }
    
}