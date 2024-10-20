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

#define BUFFER_SIZE 2048

namespace Network {

    ClientID id_counter = 1;

    //SERVER USE
    sockpp::tcp_acceptor acceptor;
    std::thread client_acceptor;
    //std::vector<std::unique_ptr<sockpp::socket>> sockets;
    std::map<ClientID, sockpp::socket> sockets;
    std::list<std::thread> threads;

    std::mutex message_queue_mut;
    std::queue< std::pair<ClientID, std::string> > message_queue;

    void acceptConnections() {
        while(acceptor) {
            ClientID client = id_counter++;
            sockets[client] = acceptor.accept().release_or_throw();
            threads.push_front(std::thread([client](){
                char buffer[BUFFER_SIZE];
                while(sockets.find(client) != sockets.end()) {
                    //TODO: CHECK IF CONNECTION IS STILL OPEN
                    size_t n = sockets[client].recv(buffer, sizeof(buffer)).value_or_throw();
                    if(n > 0) {
                        message_queue_mut.lock();
                        std::pair<ClientID, std::string> pair(client, std::string(buffer, n));
                        message_queue.push(pair);
                        message_queue_mut.unlock();
                    }
                }
            }));

        }
        std::cout << "stopped accpeting connections" << std::endl;
    }

    void openSocket(uint port) {
        acceptor = sockpp::tcp_acceptor(port);
        if(!acceptor) throw std::runtime_error("FAILED TO CREATE ACCEPTOR");
        client_acceptor = std::thread(acceptConnections);
    }

    void sendMessage(std::unique_ptr<Message> &message, ClientID id) {
        if(sockets.find(id) == sockets.end()) throw std::runtime_error("CONNECTION CLOSED or INVALID CLINET ID");
        sockets[id].send(message->toJson());
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

    //CLIENT USE
    sockpp::tcp_connector connector;

    void openConnection(std::string ip, uint port) {
        if(!connector.connect(sockpp::inet_address(ip, port)))
            throw std::runtime_error("FAILED TO OPEN CONNECTION");
    }

    void sendMessage(std::unique_ptr<Message> &message) {
        connector.send(message->toJson());
    }

    std::unique_ptr<Message> reciveMessage() {
        char buffer[BUFFER_SIZE];
        size_t n = 0;
        while (n == 0) n = connector.recv(buffer, sizeof(buffer)).value_or_throw();
        return deserialiseMessage(std::string(buffer, n));
    }
}