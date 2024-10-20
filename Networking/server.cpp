#include <iostream>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_socket.h>
#include <cassert>
#include "message.hpp"
#define NETWORKTYPE_SERVER
#include "network.hpp"
#include <unordered_set>

int main() {

    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);
        clients.insert(id);
        dynamic_cast<TestMessage*>(m.get())->x = id;
        //Network::sendMessage(m, id);
        for(auto client : clients) Network::sendMessage(m, client);
    }

    return 0;
}