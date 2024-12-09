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
#include <Networking/util.hpp>

#define BUFFER_SIZE 2048
#define MESSAGE_TIME 25

namespace Network {

    ClientID id_counter = 1;
    sockpp::tcp_acceptor acceptor;
    std::thread client_acceptor;

    std::unordered_set<ClientID> active_clients;
    std::map<ClientID, sockpp::tcp_socket> send_sockets;
    std::map<ClientID, sockpp::tcp_socket> recive_sockets;

    std::list<std::thread> threads;
    std::mutex message_queue_mut;
    std::queue< std::pair<ClientID, std::string> > message_queue;

    std::mutex close_con_mut;
    void closeConnection(ClientID client) {
        close_con_mut.lock();
        if(active_clients.find(client) == active_clients.end()) return;
        active_clients.erase(client);

        std::pair<ClientID, std::string> pair(client, RemoteDisconnectEvent().toJson());
        message_queue_mut.lock();
        message_queue.push(pair);
        message_queue_mut.unlock();
        close_con_mut.unlock();
    }

    bool clientConnected(ClientID client, bool rec) {
        if(active_clients.find(client) == active_clients.end()) return false;
        if(rec && recive_sockets[client].is_open()) return true;
        if(!rec && send_sockets[client].is_open()) return true;
        
        closeConnection(client);
        return false;
    }

    void acceptConnections() {
        while(acceptor) {
            //accept all incomming connections and get first message
            auto socket_res = acceptor.accept();
            if(socket_res.is_error()) continue;
            sockpp::tcp_socket socket = socket_res.release();
            char buffer[BUFFER_SIZE];
            size_t n = 0;
            bool err = false;
            while (n == 0 && !err) {
                auto n_res = socket.recv(buffer, sizeof(buffer));
                if(n_res.is_error()) err = true;
                else n = n_res.value();
            }
            if(err) continue;
            auto message = std::string(buffer, n);
            
            //open recive connection and hand out ClientID
            if(message == "request id") {
                ClientID client = id_counter++;
                socket.send(std::to_string(client));
                recive_sockets[client] = std::move(socket);
            }

            //open send connection and finalize connection protocol
            if(message.substr(0,7) == "recive ") {
                ClientID client = std::stoi(message.substr(7));
                send_sockets[client] = std::move(socket);
                active_clients.insert(client);

                //create new recive connection thread
                threads.push_back(std::thread([client]() {
                    char buffer[BUFFER_SIZE];
                    while(clientConnected(client, true)) {
                        auto size = recive_sockets[client].recv(buffer, sizeof(buffer));
                        if(size.is_error() || size.value() == 0) { //Network Error / client disconnect
                            closeConnection(client);
                            continue;
                        }
                        std::pair<ClientID, std::string> pair(client, std::string(buffer, size.value()));
                        message_queue_mut.lock();
                        message_queue.push(pair);
                        message_queue_mut.unlock();
                    }
                    //evtl da no recive sockets entry lösche (problematisch mit thread saftey)
                }));
            }
        }
    }

    void openSocket(uint port) {
        acceptor = sockpp::tcp_acceptor();
        acceptor.open(port, 4, SO_REUSEPORT);
        if(!acceptor) THROW_ERROR("FAILED TO CREATE ACCEPTOR");
        client_acceptor = std::thread(acceptConnections);
    }

    std::chrono::system_clock::time_point last_message;
    bool sendMessage(std::unique_ptr<Message> message, ClientID id) {
        if(!clientConnected(id, false)) return false;

        //Message frequency limiter 
        auto now = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_message).count();
        if(delta < MESSAGE_TIME) std::this_thread::sleep_for(std::chrono::milliseconds(MESSAGE_TIME - delta));
        last_message = std::chrono::system_clock::now();

        auto res = send_sockets[id].send(message->toJson());
        std::cout << "(network debug) SENDING: " << message->toJson() << std::endl;
        if(res.is_error() || res.value() == -1) {
            closeConnection(id);
            return false;
        }
        return true;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(MESSAGE_TIME));
        }
    }
    
}