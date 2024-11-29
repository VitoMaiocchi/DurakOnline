#include "../include/server.hpp"
#include <unordered_set>
#include <iostream>



void handleMessage(std::unique_ptr<Message> msg_r, ClientID client){
    //da message handle dies das
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

        case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
            // Client connected, add name to a datastructure maybe a map
            if (clients.find(client) == clients.end() && clients.size() < MAX_PLAYERS) {
                clients.insert(client);
                std::cout << "New client connected: " << client << std::endl;

                Player p; //is there a way to name every player differently?
                ClientConnectEvent* connect = dynamic_cast<ClientConnectEvent*>(msg_r.get());
                p.name = connect->username;
                p.player_id = client;
                std::cout << "Client name: " << p.name <<  std::endl;

            }
            // Gamestate update message
            GameStateUpdate update;
            update.state = GAMESTATE_LOBBY;
            Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
            std::cout << std::endl;


            break;
        }

        case MESSAGETYPE_CLIENT_ACTION_EVENT: {
            // Example: Ready action from the client
            ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
            if (action && action->action == CLIENTACTION_READY) {
                ready_clients.insert(client);
                std::cout << "Client " << client << " is ready." << std::endl;
                std::cout << std::endl;
            }

            // Check if enough and all players are ready to start the game
            if (ready_clients.size() >= MIN_PLAYERS && ready_clients.size() == clients.size() && current_game == nullptr ) {
                std::cout << "Starting a new game..." << std::endl;
                std::vector<ClientID> player_ids(ready_clients.begin(), ready_clients.end());
                current_game = std::make_unique<Game>(player_ids);
                GameStateUpdate update;
                update.state = GAMESTATE_GAME;
                Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
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
            ready_clients.erase(client);

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