#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"
#include <unordered_set>
#include <iostream>


namespace DurakServer{
    std::unordered_set<ClientID> clients;
    std::unordered_set<ClientID> ready_clients;
    std::unique_ptr<Game> current_game = nullptr;
}



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

    while(true) {
        //listen for messages
        ClientID client;
        std::unique_ptr msg_r = Network::reciveMessage(client); //receiving message
        if(msg_r == nullptr){
            std::cerr << "null ptr received" <<std::endl;
            return -1;
        }

        std::cout << "Message received from client: " << client << std::endl;

        handleMessage(std::move(msg_r), client);

    }
    return 0;

}
