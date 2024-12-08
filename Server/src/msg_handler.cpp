#include "../include/server.hpp"



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
            PlayerUpdate player_update;
            if (clients.find(client) == clients.end() && clients.size() < MAX_PLAYERS) {
                clients.insert(client);
                std::cout << "New client connected: " << client << std::endl;

                Player p; //is there a way to name every player differently?
                ClientConnectEvent* connect = dynamic_cast<ClientConnectEvent*>(msg_r.get());
                p.name = connect->username;
                p.player_id = client;
                p.durak = false;
                std::cout << "Client name: " << p.name <<  std::endl;

                DurakServer::players_map[client] = p;

                for(auto c : players_map){
                    player_update.player_names[c.first] = c.second.name;
                }
                
                // Gamestate update message
                GameStateUpdate update;
                update.state = GAMESTATE_LOBBY;
                Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
                std::cout << std::endl;

                player_update.player_count = players_map.size();
                player_update.durak = 0;

                std::vector<ClientID> player_ids(clients.begin(), clients.end());

                for(auto c : player_ids){
                    Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
                }
            }
            else{
                std::cerr << "Failed to connect the client"<<std::endl;
            }

            break;
        }

        case MESSAGETYPE_CLIENT_ACTION_EVENT: {
            // Example: Ready action from the client
            ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
            if (action && action->action == CLIENTACTION_READY) {
                ready_clients.insert(client);
                std::cout << "Client " << client << " is ready." << std::endl;
                std::cout << std::endl;
            

                // Check if enough and all players are ready to start the game
                if (ready_clients.size() >= MIN_PLAYERS && ready_clients.size() == clients.size() && current_game == nullptr ) {
                    std::cout << "Starting a new game..." << std::endl;
                    std::vector<ClientID> player_ids(ready_clients.begin(), ready_clients.end());
                    std::sort(player_ids.begin(), player_ids.end());
                    GameStateUpdate update;
                    update.state = GAMESTATE_GAME;
                    for(auto c : player_ids){
                        Network::sendMessage(std::make_unique<GameStateUpdate>(update), c);
                    }
                    current_game = std::make_unique<Game>(player_ids);
                    //searching bug 
                    std::cout << "player ids vector with which the game is initialized \nid: "<< std::endl;
                    for(auto p : player_ids){
                        std::cout << p << " ";
                    }
                }
            }
            else if(action && action->action == CLIENTACTION_PICK_UP || 
                                action->action == CLIENTACTION_PASS_ON ||
                                action->action == CLIENTACTION_OK){
                std::cout << "pick up message received from client: " << client << std::endl;
                if(current_game){
                    current_game->handleClientActionEvent(std::move(msg_r), client);
                }
                else{
                    std::cerr << "No active game to handle action event" << std::endl;
                }
            }
            else{
                std::cerr << "game exists already or not all players ready" << std::endl;
            }
            break;
        }

        case MESSAGETYPE_PLAYCARD_EVENT: {
            // Handle card play event if a game is active
            if (current_game) {
                if(!current_game->endGame()){
                    current_game->handleClientCardEvent(std::move(msg_r), client);
                }
            
                else if(current_game->endGame()){
                    //clean up
                    //player update first
                    PlayerUpdate player_update;
                    player_update.durak = 0; //getlastplayer
                    player_update.player_count = clients.size();
                    
                    GameStateUpdate game_update;
                    game_update.state = GAMESTATE_DURAK_SCREEN;

                    for(auto c : clients){
                        Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
                        Network::sendMessage(std::make_unique<GameStateUpdate>(game_update), c);
                        // ready_clients.erase(c);
                    }
                }
            }
            else {
                std::cerr << "No active game to handle play card event!" << std::endl;
            }
            break;
        }

        case MESSAGETYPE_REMOTE_DISCONNECT_EVENT: {
            std::cout << "Client disconnected: " << client << std::endl;
            clients.erase(client);
            ready_clients.erase(client);
            players_map.erase(client);
            if(players_map.size() < 2){
                current_game.reset();
                GameStateUpdate game_update;
                game_update.state = GAMESTATE_LOBBY;
                for(auto c : ready_clients){
                    //send message to send the players into the lobby
                    Network::sendMessage(std::make_unique<GameStateUpdate>(game_update), c);
                }
            }
            PlayerUpdate player_update;
            player_update.player_count = players_map.size();
            player_update.durak = 0;
            for(ClientID c : clients){
                player_update.player_names[c] = players_map[c].name;
            }
            for(ClientID c : clients){
                Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
            }
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