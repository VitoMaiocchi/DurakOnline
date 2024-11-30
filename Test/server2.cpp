#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../Server/include/server.hpp"
#include "../Server/include/game.hpp"
#include <unordered_set>
#include <iostream>


namespace DurakServer{
    std::unordered_set<ClientID> clients;
    std::unordered_set<ClientID> ready_clients;
    std::unique_ptr<Game> current_game = nullptr;
    std::map<ClientID, Player> players_map;
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

        using namespace DurakServer;
        switch (msg_r->messageType) {
            case MESSAGETYPE_TEST: {
                std::cout << "just a test message from client: " << client <<std::endl;
                // TestMessage* return_test = dynamic_cast<TestMessage*>(message.get());
                TestMessage* ret_test = dynamic_cast<TestMessage*>(msg_r.get());
                std::cout << "string: " << ret_test->string
                        << "\nx: " <<ret_test->x
                        << "\ny: " <<ret_test->y << std::endl;
                dynamic_cast<TestMessage*>(msg_r.get())->x = client;
                Network::sendMessage(std::move(msg_r), client);
            }
            break;
            // the client connects to the server and enters his username
            // in return he will be transfered to the lobby and his name is saved on the server
            case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
                //player update message prep
                PlayerUpdate player_update;
                // Client connected, add name to a datastructure maybe a map
                if (clients.find(client) == clients.end() && clients.size() < MAX_PLAYERS) {
                    clients.insert(client);
                    std::cout << "New client connected: " << client << std::endl;

                    Player p; //is there a way to name every player differently?
                    ClientConnectEvent* connect = dynamic_cast<ClientConnectEvent*>(msg_r.get());
                    p.name = connect->username;
                    p.player_id = client;
                    p.durak = false; //default value for new players

                    std::cout << "Client name: " << p.name <<  std::endl;
                    // store the player object in the map
                    DurakServer::players_map[client] = p;
                    //add player to update msg 
                    for(auto c : players_map){
                        player_update.player_names[c.first] = c.second.name;
                    } 
                }


                // Gamestate update message
                GameStateUpdate update;
                update.state = GAMESTATE_LOBBY;
                Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
                std::cout << std::endl;

                //this is added for testing purposes to start the game screen
                if(clients.size() >= MIN_PLAYERS){
                    player_update.player_count = players_map.size(); //playerupdate msg

                    std::vector<ClientID> player_ids(clients.begin(), clients.end());
                    GameStateUpdate update;
                    update.state = GAMESTATE_GAME;
                    for(auto c : player_ids){
                        Network::sendMessage(std::make_unique<GameStateUpdate>(update), c);
                        Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
                    }

                    current_game = std::make_unique<Game>(player_ids);
                    std::cout << "Starting a new game..." << std::endl;
                }

                break;
            }

            // this is removed for testing purposes
            case MESSAGETYPE_CLIENT_ACTION_EVENT: {
                // Example: Ready action from the client
                ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
                if (action && action->action == CLIENTACTION_READY) {
                    ready_clients.insert(client);
                    std::cout << "Client " << client << " is ready." << std::endl;
                    std::cout << std::endl;
                // Check if enough and all players are ready to start the game
                    // if (ready_clients.size() >= MIN_PLAYERS && current_game == nullptr ) {
                    //     std::cout << "Starting a new game..." << std::endl;
                    //     std::vector<ClientID> player_ids(clients.begin(), clients.end());
                    //     current_game = std::make_unique<Game>(player_ids);
                    //     GameStateUpdate update;
                    //     update.state = GAMESTATE_GAME;
                    //     Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
                    // }
                }
                else if(action && action->action == CLIENTACTION_PICK_UP || 
                                  action->action ==CLIENTACTION_PASS_ON ||
                                  action->action == CLIENTACTION_OK){
                    std::cout << "pick up message received from client: " << client << std::endl;
                    if(current_game){
                        current_game->handleClientActionEvent(std::move(msg_r), client);
                    }
                    else{
                        std::cerr << "No active game to handle action event" << std::endl;
                    }
                }
                else {
                    std::cout << "unidentified action message received";
                }
                break;
            }

            case MESSAGETYPE_PLAYCARD_EVENT: {
                // Handle card play event if a game is active
                if (current_game) {
                    current_game->handleClientCardEvent(std::move(msg_r), client);
                } else {
                    std::cerr << "No active game to handle play card event!" << std::endl;
                }
                break;
            }

            case MESSAGETYPE_REMOTE_DISCONNECT_EVENT: {
                std::cout << "Client disconnected: " << client << std::endl;
                clients.erase(client);
                // ready_clients.erase(client); removed for testing purposes

                // Handle cleanup if a client disconnects mid-game
                if (current_game) {
                    // Implement logic to handle a player leaving
                    // e.g., burn their cards, adjust turn order, etc.
                }
                break;
            }

            default: {
                std::cerr << "Unhandled message type: " << msg_r->messageType << std::endl;
                break;
            }
        }

    }
    return 0;

}