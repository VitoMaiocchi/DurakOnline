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
            
            if(clients.size() >= MAX_PLAYERS || current_game) {
                RemoteDisconnectEvent event;
                Network::sendMessage(std::make_unique<RemoteDisconnectEvent>(event), client);
                break;
            }

            // Client connected, add name to a datastructure maybe a map
            PlayerUpdate player_update;
            if (clients.find(client) == clients.end() && clients.size() <= MAX_PLAYERS) {
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

                ReadyUpdate rupdate;
                rupdate.players = ready_clients;
                Network::sendMessage(std::make_unique<ReadyUpdate>(rupdate), client);
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
            
                ReadyUpdate update;
                update.players = ready_clients;
                for(ClientID c : clients) Network::sendMessage(std::make_unique<ReadyUpdate>(update), c);

                // Check if enough and all players are ready to start the game
                if (ready_clients.size() >= MIN_PLAYERS && ready_clients.size() == clients.size() && current_game == nullptr ) {
                    std::cout << "Starting a new game..." << std::endl;
                    std::set<ClientID> player_ids(ready_clients.begin(), ready_clients.end());
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
            else if(action && action->action == CLIENTACTION_LOBBY){
                GameStateUpdate game_state;
                game_state.state = GAMESTATE_LOBBY;

                ready_clients.erase(client);
                Network::sendMessage(std::make_unique<GameStateUpdate>(game_state),client);
            }
            else{
                std::cerr << "game exists already or not all players ready" << std::endl;
            }
            break;
        }

        case MESSAGETYPE_PLAYCARD_EVENT: {
            // Handle card play event if a game is active
            if (current_game) {
                // handleClientCardEvent() returns 0 if the event was passed on successfully (game is not yet finished and there is no durak)
                // and it returns the duraks id if the game is finished
                ClientID durak = current_game->handleClientCardEvent(std::move(msg_r), client);
                if(durak){
                    //clean up
                    //player update first
                    PlayerUpdate player_update;
                    player_update.durak = durak; //getlastplayer
                    players_map[durak].durak = true;
                    player_update.player_count = players_map.size();
                    for(auto player : players_map){
                        player_update.player_names[player.first] = player.second.name;
                    }
                    
                    GameStateUpdate normal_update;
                    normal_update.state = GAMESTATE_GAME_OVER;

                    GameStateUpdate durak_update;
                    durak_update.state = GAMESTATE_DURAK_SCREEN;


                    for(auto c : clients){
                        if(c == durak){
                            Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
                            Network::sendMessage(std::make_unique<GameStateUpdate>(durak_update), c);
                        } else {
                            Network::sendMessage(std::make_unique<PlayerUpdate>(player_update), c);
                            Network::sendMessage(std::make_unique<GameStateUpdate>(normal_update), c);
                        }
                    }
                    ReadyUpdate ready_update;
                    for(auto c : clients){
                        ready_clients.erase(c); //unready the clients in the lobby so they restart the game
                    }
                    ready_update.players = ready_clients; // should be empty
                    for(ClientID c : clients){
                        Network::sendMessage(std::make_unique<ReadyUpdate>(ready_update), c);
                    }
                    std::cout << "Game ended, durak found" << durak << std::endl;
                    // delete current game
                    current_game.reset();
                    
                    
                }
            } else {
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
                // current_game->
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