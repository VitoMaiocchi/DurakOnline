#include "../include/msg_handler.hpp"
#include <unordered_set>
#include <iostream>


// Minimum number of players required to start a game
constexpr size_t MIN_PLAYERS = 3;

void handleMessage(std::unique_ptr<Message> msg_r, ClientID client, std::unique_ptr<Game>& current_game, std::unordered_set<ClientID> clients, std::unordered_set<ClientID>& ready_clients){
    //da message handle dies das
    switch (msg_r->messageType) {
        case MESSAGETYPE_TEST: {
            std::cout << "just a test message from client: " << client <<std::endl;
            // TestMessage* return_test = dynamic_cast<TestMessage*>(message.get());
            TestMessage* ret_test = dynamic_cast<TestMessage*>(msg_r.get());
            std::cout << "string: " << ret_test->string
                      << "\nx: " <<ret_test->x
                      << "\ny: " <<ret_test->y << std::endl;
            dynamic_cast<TestMessage*>(msg_r.get())->x = client;
            Network::sendMessage(msg_r, client);
        }
        break;

        case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
            // Client connected; update readiness or broadcast
            std::cout << "Client connected: " << client << std::endl;
            break;
        }

        case MESSAGETYPE_CLIENT_ACTION_EVENT: {
            // Example: Ready action from the client
            ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
            if (action && action->action == CLIENTACTION_READY) {
                ready_clients.insert(client);
                std::cout << "Client " << client << " is ready." << std::endl;
            }

            // Check if enough players are ready to start the game
            if (ready_clients.size() >= MIN_PLAYERS && current_game == nullptr) {
                std::cout << "Starting a new game..." << std::endl;
                std::vector<ClientID> player_ids(ready_clients.begin(), ready_clients.end());
                current_game = std::make_unique<Game>(player_ids);
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

        case MESSAGETYPE_CLIENT_DISCONNECT_EVENT: {
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