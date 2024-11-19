#include <iostream>
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <unordered_set>

int main() {
    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);
        if(m == nullptr){
            std::cerr << "null ptr received" <<std::endl;
            return -1;
        }
        else{
            std::cout << "message received" << std::endl;
        }
        clients.insert(id);

        switch (m->messageType) {
            case MESSAGETYPE_TEST:
                dynamic_cast<TestMessage*>(m.get())->x = id;
                for(auto client : clients) {
                    Network::sendMessage(m, client);
                    std::cout << "message sending with type: " << m->messageType << std::endl;
                }
            break;

            case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
                std::cout << "CLIENT DISCONNECTED: "<<id << std::endl;
            break;
        }
    }




    return 0;
}