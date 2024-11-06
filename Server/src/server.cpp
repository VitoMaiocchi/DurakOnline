#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"
#include <unordered_set>
#include <iostream>

Game* currentGame = nullptr;

struct Player{
    //string name
    //dies das
};

//zb map<ClientID, player(username ka)> 
//state (lobby, game, etc)

void broadcastMessage(std::unique_ptr<Message> message) {
    //for all in map
    //send Network::message
}

void handleMessage(std::unique_ptr<Message> message, ClientID client) {
    //da message handle dies das
    switch (message->messageType) {
        MESSAGETYPE_CARD_UPDATE:
            //do something
            //Network::sendMessage(client, goo goo gagag);
        break;
        //...
    }
}

int main() {

    //start networking
    Network::openSocket(42069);
    //set up irgend welches züg etc

    while(true) {
        //listen for messages
        ClientID client;
        handleMessage(Network::reciveMessage(client), client);
    }
    return 0;

}

// int main() {
//     Network::openSocket(42069);
//     std::unordered_set<ClientID> clients;
//     while(true) {
//         ClientID id;
//         std::unique_ptr<Message> m = Network::reciveMessage(id);
//         if(m == nullptr){
//             std::cerr << "null ptr received" <<std::endl;
//             return -1;
//         }
//         else{
//             std::cout << "message received" << std::endl;
//         }
//         clients.insert(id);
//         switch (m->messageType) {
//             case MESSAGETYPE_TEST:
//                 dynamic_cast<TestMessage*>(m.get())->x = id;
//                 for(auto client : clients) {
//                     Network::sendMessage(m, client);
//                     std::cout << "message sending with type: " << m->messageType << std::endl;
//                 }
//             break;
//             case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
//                 std::cout << "CLIENT DISCONNECTED: "<<id << std::endl;
//             break;
//         }
//     }

//     return 0;
// }