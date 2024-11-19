#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"
#include "../include/msg_handler.hpp"
#include <unordered_set>
#include <iostream>


// Track connected clients
std::unordered_set<ClientID> clients;

// Track ready clients
std::unordered_set<ClientID> ready_clients;

// Game instance
std::unique_ptr<Game> current_game = nullptr;

// Minimum number of players required to start a game
constexpr size_t MIN_PLAYERS = 3;
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

    while(true) {
        //listen for messages
        ClientID client;
        std::unique_ptr msg_r = Network::reciveMessage(client); //receiving message
        if(msg_r == nullptr){
            std::cerr << "null ptr received" <<std::endl;
            return -1;
        }

        std::cout << "Message received from client: " << client << std::endl;

        // Handle new client connections
        if (clients.find(client) == clients.end()) {
            clients.insert(client);
            std::cout << "New client connected: " << client << std::endl;
        }
        handleMessage(std::move(msg_r), client, current_game, clients, ready_clients);
        // switch (msg_r->messageType) {
        //     case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
        //         // Client connected; update readiness or broadcast
        //         std::cout << "Client connected: " << client << std::endl;
        //         break;
        //     }

        //     case MESSAGETYPE_CLIENT_ACTION_EVENT: {
        //         // Example: Ready action from the client
        //         ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
        //         if (action && action->action == CLIENTACTION_READY) {
        //             ready_clients.insert(client);
        //             std::cout << "Client " << client << " is ready." << std::endl;
        //         }

        //         // Check if enough players are ready to start the game
        //         if (ready_clients.size() >= MIN_PLAYERS && current_game == nullptr) {
        //             std::cout << "Starting a new game..." << std::endl;
        //             std::vector<ClientID> player_ids(ready_clients.begin(), ready_clients.end());
        //             current_game = std::make_unique<Game>(player_ids);
        //         }
        //         break;
        //     }

        //     case MESSAGETYPE_PLAYCARD_EVENT: {
        //         // Handle card play event if a game is active
        //         if (current_game) {
        //             current_game->handleClientCardEvent(std::move(msg_r), client);
        //         } else {
        //             std::cerr << "No active game to handle play card event!" << std::endl;
        //         }
        //         break;
        //     }

        //     case MESSAGETYPE_CLIENT_DISCONNECT_EVENT: {
        //         std::cout << "Client disconnected: " << client << std::endl;
        //         clients.erase(client);
        //         ready_clients.erase(client);

        //         // Handle cleanup if a client disconnects mid-game
        //         if (current_game) {
        //             // Implement logic to handle a player leaving
        //             // e.g., burn their cards, adjust turn order, etc.
        //         }
        //         break;
        //     }

        //     default: {
        //         std::cerr << "Unhandled message type: " << msg_r->messageType << std::endl;
        //         break;
        //     }
        // }

        // if all clients have pressed ready, start the game
        // std::vector<ClientID> player_ids;
        // for(auto client : clients){
        //     player_ids.push_back(client);
        // }
        // Game current_game(player_ids);
        
        // handleMessage(std::move(msg_r), client, nullptr);
        // handleMessage(std::move(msg_r), client, current_game /*, clients*/);
        
        //the handleMessage send message will be called somewhere else
    }
    return 0;

}
