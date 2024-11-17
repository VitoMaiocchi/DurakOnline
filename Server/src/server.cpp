#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"
#include "../include/msg_handler.hpp"
#include <unordered_set>
#include <iostream>


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

int main() {

    //start networking
    Network::openSocket(42069);
    //set up irgend welches züg etc
    std::unordered_set<ClientID> clients;

    while(true) {
        //listen for messages
        ClientID client;
        std::unique_ptr msg_r = Network::reciveMessage(client); //receiving message
        if(msg_r == nullptr){
            std::cerr << "null ptr received" <<std::endl;
            return -1;
        }
        else{
            std::cout << "message received" << std::endl;
        }
        clients.insert(client);

        // if all clients have pressed ready, start the game
        std::vector<ClientID> player_ids;
        for(auto client : clients){
            player_ids.push_back(client);
        }
        Game current_game(player_ids);
        

        handleMessage(std::move(msg_r), client, &current_game /*, clients*/);
        
        //the handleMessage send message will be called somewhere else
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

    // std::cout << "TEST MESSAGETYPE_TEST"<<std::endl;
    // TestMessage message;
    // message.x = 3;
    // message.y = 7;
    // message.string = "mhh trash i like trash";
    // std::unique_ptr<Message> m = std::make_unique<TestMessage>(message);
    // Network::openConnection("localhost", 42069);
    // while(true) {
    //     Network::sendMessage(m);
    //     std::unique_ptr<Message> awnser = nullptr;
    //     while(!awnser) awnser = Network::reciveMessage();
    //     TestMessage* ret = dynamic_cast<TestMessage*>(awnser.get());
    //     std::cout   << "string: " << ret->string
    //                 << "\nx: "<< ret->x << std::endl;

    // }