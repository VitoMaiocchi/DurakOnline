#include <iostream>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_socket.h>
#include <cassert>
#include "message.hpp"
#define NETWORKTYPE_SERVER
#include "network.hpp"
#include <unordered_set>
#include "util.hpp"

int main() {

    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);
        clients.insert(id);
        //PRINT_DEBUG("START");
        switch (m->messageType) {
            case MESSAGETYPE_TEST:
                dynamic_cast<TestMessage*>(m.get())->x = id;
                for(auto client : clients) Network::sendMessage(m, client);
                //PRINT_DEBUG("MESSAGE");
            break;
            case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
                //PRINT_DEBUG("DISCONNECT");
                std::cout << "CLIENT DISCONNECTED: "<<id << std::endl;
            break;
            //default:
                //PRINT_DEBUG("DEFAULT");
        }
    }

    //PRINT_DEBUG("IDK");

    return 0;
}