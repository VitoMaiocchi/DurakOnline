#include "network.hpp"
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

namespace Network {

    //SERVER USE

    ClientID id_counter = 1;
    sockpp::tcp_acceptor acceptor;
    std::thread client_acceptor;
    //std::vector<std::unique_ptr<sockpp::socket>> sockets;
    std::unordered_set<ClientID> active_clients;
    std::map<ClientID, sockpp::socket> send_sockets;
    std::map<ClientID, sockpp::socket> recive_sockets;

    std::list<std::thread> threads;
    std::mutex message_queue_mut;
    std::queue< std::pair<ClientID, std::string> > message_queue;

    void acceptConnections() {
        while(acceptor) {
            sockpp::socket socket = acceptor.accept().release_or_throw();

            char buffer[BUFFER_SIZE];
            size_t n = 0;
            while (n == 0) n = socket.recv(buffer, sizeof(buffer)).value_or_throw();
            auto message = std::string(buffer, n);
            
            if(message == "request id") {
                ClientID client = id_counter++;
                socket.send(std::to_string(client));
                recive_sockets[client] = std::move(socket);
                std::cout << "open recive connection with " << client << std::endl;
            }

            std::cout << "full message ["<<message<<"]" <<std::endl;
            std::cout << "message ["<<message.substr(0,7)<<"]" <<std::endl;
            if(message.substr(0,7) == "recive ") {
                std::cout << "opening send connection with [" << message.substr(7) <<"]"<< std::endl;
                ClientID client = std::stoi(message.substr(7));
                send_sockets[client] = std::move(socket);
                active_clients.insert(client);
                std::cout << "open send connection with " << client << std::endl;

                threads.push_back(std::thread([client]() {
                    std::cout << "reciving connections for " << client << std::endl;
                    char buffer[BUFFER_SIZE];
                    while(active_clients.find(client) != active_clients.end()) {
                        size_t n = recive_sockets[client].recv(buffer, sizeof(buffer)).value_or_throw();
                        if(n > 0) {
                            message_queue_mut.lock();
                            std::pair<ClientID, std::string> pair(client, std::string(buffer, n));
                            std::cout << "message recived: [" << std::string(buffer, n) <<
                                "] from client: " << client << std::endl;
                            message_queue.push(pair);
                            message_queue_mut.unlock();
                        }
                    }
                }));
            }
        }
        std::cout << "stopped accpeting connections" << std::endl;
    }

    void openSocket(uint port) {
        acceptor = sockpp::tcp_acceptor(port);
        if(!acceptor) throw std::runtime_error("FAILED TO CREATE ACCEPTOR");
        client_acceptor = std::thread(acceptConnections);
    }

    void sendMessage(std::unique_ptr<Message> &message, ClientID id) {
        if(active_clients.find(id) == active_clients.end()) throw std::runtime_error("CONNECTION CLOSED or INVALID CLINET ID");
        send_sockets[id].send(message->toJson());
    }

    std::unique_ptr<Message> reciveMessage(ClientID &id) {
        while(true) {
            message_queue_mut.lock();
            if(!message_queue.empty()) {
                auto pair = message_queue.front();
                message_queue.pop();
                message_queue_mut.unlock();
                id = pair.first;
                return deserialiseMessage(pair.second);
            }
            message_queue_mut.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }
    
}