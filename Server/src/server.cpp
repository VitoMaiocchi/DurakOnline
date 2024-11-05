#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include "server.hpp"
#include "game.hpp"

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

}