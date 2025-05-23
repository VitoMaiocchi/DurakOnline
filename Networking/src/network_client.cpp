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
    bool connected = false;

    //CLIENT USE
    sockpp::tcp_connector send_connector;
    sockpp::tcp_connector recive_connector;
    ClientID client_id;
    std::mutex message_queue_mut;
    std::queue<std::string> message_queue;
    std::thread recive_thread;

    void closeConnection() {
        if(!connected) return;
        connected = false;
        message_queue_mut.lock();
        message_queue.push(RemoteDisconnectEvent().toJson());
        message_queue_mut.unlock();
        recive_connector.close();
        send_connector.close();
    }
    
    ClientID openConnection(std::string ip, uint port) {
        if (connected) {
            std::cerr << "Already connected. Disconnecting first..." << std::endl;
            closeConnection();
        }
        assert(!connected); //cannt connect twice

        std::cout << "opening connection..." << std::endl;

        //reset everything incase it was allready connected
        if(recive_thread.joinable()) recive_thread.join();
        send_connector = sockpp::tcp_connector();
        recive_connector = sockpp::tcp_connector();

        try {
            //3 seconds connect timeout
            if(!send_connector.connect(ip, port, std::chrono::milliseconds(3000))) return 0;
        } catch (...) {
            return 0;
        }
        send_connector.send("request id");
        char buffer[BUFFER_SIZE];
        size_t n = 0;
        while (n == 0) n = send_connector.recv(buffer, sizeof(buffer)).value_or_throw();
        client_id = std::stoi(std::string(buffer, n));
        std::cout << "client id ["<< client_id<<"]" << std::endl;
        if(!recive_connector.connect(sockpp::inet_address(ip, port))) return 0;
        recive_connector.send("recive " + std::to_string(client_id));
        recive_connector.read_timeout(std::chrono::microseconds(10000));
        connected = true;
        recive_thread = std::thread([](){
            char buffer[BUFFER_SIZE];
            while(connected) {
                auto r = recive_connector.recv(buffer, sizeof(buffer));
                if(r.error()) continue; //PROBABLY TIMEOUT
                size_t n = r.value();
                if(n == 0) { //disconnect
                    message_queue_mut.lock();
                    message_queue.push(RemoteDisconnectEvent().toJson());
                    message_queue_mut.unlock();
                    recive_connector.close();
                    send_connector.close();
                    connected = false;
                    return;
                }
                message_queue_mut.lock();
                std::cout << "(network debug) RECIVEING: " << std::string(buffer,n) << std::endl;
                message_queue.push(std::string(buffer,n));
                message_queue_mut.unlock();
            }
        });
        std::cout << "connection established" << std::endl;

        return client_id;
    }



    std::chrono::system_clock::time_point last_message;
    void sendMessage(std::unique_ptr<Message> message) {
        if(!connected) {
            std::cout << "WARNING: trying to send message while disconnected" << std::endl;
            return;
        }

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