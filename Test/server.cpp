// #define NETWORKTYPE_SERVER
// #include <Networking/network.hpp>
// #include <Networking/message.hpp>

// #include "../Server/include/server.hpp"
// #include "../Server/include/game.hpp"
// #include <unordered_set>
// #include <iostream>


// namespace DurakServer{
//     std::unordered_set<ClientID> clients;
//     std::unordered_set<ClientID> ready_clients;
//     std::unique_ptr<Game> current_game = nullptr;
// }



// //zb map<ClientID, player(username ka)> 
// //state (lobby, game, etc)

// void broadcastMessage(std::unique_ptr<Message> message) {
//     //for all in map
//     //send Network::message
// }

// int main() {

//     //start networking
//     Network::openSocket(42069);
//     //set up irgend welches züg etc

//     while(true) {
//         //listen for messages
//         ClientID client;
//         std::unique_ptr msg_r = Network::reciveMessage(client); //receiving message
//         if(msg_r == nullptr){
//             std::cerr << "null ptr received" <<std::endl;
//             return -1;
//         }

//         std::cout << "Message received from client: " << client << std::endl;

//         using namespace DurakServer;
//         switch (msg_r->messageType) {
//             case MESSAGETYPE_TEST: {
//                 std::cout << "just a test message from client: " << client <<std::endl;
//                 // TestMessage* return_test = dynamic_cast<TestMessage*>(message.get());
//                 TestMessage* ret_test = dynamic_cast<TestMessage*>(msg_r.get());
//                 std::cout << "string: " << ret_test->string
//                         << "\nx: " <<ret_test->x
//                         << "\ny: " <<ret_test->y << std::endl;
//                 dynamic_cast<TestMessage*>(msg_r.get())->x = client;
//                 Network::sendMessage(std::move(msg_r), client);
//             }
//             break;

//             case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
//                 // Client connected, add name to a datastructure maybe a map
//                 if (clients.find(client) == clients.end() && clients.size() < MAX_PLAYERS) {
//                     clients.insert(client);
//                     std::cout << "New client connected: " << client << std::endl;

//                     Player p; //is there a way to name every player differently?
//                     ClientConnectEvent* connect = dynamic_cast<ClientConnectEvent*>(msg_r.get());
//                     p.name = connect->username;
//                     p.player_id = client;
//                     std::cout << "Client name: " << p.name <<  std::endl;

//                 }
//                 // Gamestate update message
//                 GameStateUpdate update;
//                 update.state = GAMESTATE_LOBBY;
//                 Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
//                 std::cout << std::endl;
//                 if(clients.size() >= MIN_PLAYERS){
//                     std::vector<ClientID> player_ids(clients.begin(), clients.end());
//                     current_game = std::make_unique<Game>(player_ids);
//                     std::cout << "Starting a new game..." << std::endl;
//                     GameStateUpdate update;
//                     update.state = GAMESTATE_GAME;
//                     Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
//                 }

//                 break;
//             }

//             case MESSAGETYPE_CLIENT_ACTION_EVENT: {
//                 // Example: Ready action from the client
//                 // ClientActionEvent* action = dynamic_cast<ClientActionEvent*>(msg_r.get());
//                 // if (action && action->action == CLIENTACTION_READY) {
//                 //     ready_clients.insert(client);
//                 //     std::cout << "Client " << client << " is ready." << std::endl;
//                 //     std::cout << std::endl;
//                 // // Check if enough and all players are ready to start the game
//                 //     if (ready_clients.size() >= MIN_PLAYERS && current_game == nullptr ) {
//                 //         std::cout << "Starting a new game..." << std::endl;
//                 //         std::vector<ClientID> player_ids(clients.begin(), clients.end());
//                 //         current_game = std::make_unique<Game>(player_ids);
//                 //         GameStateUpdate update;
//                 //         update.state = GAMESTATE_GAME;
//                 //         Network::sendMessage(std::make_unique<GameStateUpdate>(update), client);
//                 //     }
//                 // }
//                 // else {
//                 //     std::cout << "action message received but it is not ready";
//                 // }
//                 break;
//             }

//             case MESSAGETYPE_PLAYCARD_EVENT: {
//                 // Handle card play event if a game is active
//                 if (current_game) {
//                     current_game->handleClientCardEvent(std::move(msg_r), client);
//                 } else {
//                     std::cerr << "No active game to handle play card event!" << std::endl;
//                 }
//                 break;
//             }

//             case MESSAGETYPE_CLIENT_DISCONNECT_EVENT: {
//                 std::cout << "Client disconnected: " << client << std::endl;
//                 clients.erase(client);
//                 ready_clients.erase(client);

//                 // Handle cleanup if a client disconnects mid-game
//                 if (current_game) {
//                     // Implement logic to handle a player leaving
//                     // e.g., burn their cards, adjust turn order, etc.
//                 }
//                 break;
//             }

//             default: {
//                 std::cerr << "Unhandled message type: " << msg_r->messageType << std::endl;
//                 break;
//             }
//         }

//     }
//     return 0;

// }


#include <iostream>
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <unordered_set>

int main() {

    assert(Card(Card(RANK_QUEEN, SUIT_SPADES).toInt()) == Card(RANK_QUEEN, SUIT_SPADES));

    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);

        std::cout << "RECIVED MESSAGE FROM "<<id<<": \n" << m->toJson() << std::endl; 

        if(m->messageType == MESSAGETYPE_CLIENT_CONNECT_EVENT) {
            GameStateUpdate update;
            update.state = GAMESTATE_GAME;
            Network::sendMessage(std::make_unique<GameStateUpdate>(update), id);

            PlayerUpdate update3;
            update3.player_names[8] = "Garbage Goober";
            update3.player_names[7] = "Booger Eater";
            update3.player_names[10] = "Milfhunter";
            update3.player_names[id] = "YOU";

            Network::sendMessage(std::make_unique<PlayerUpdate>(update3), id);

            CardUpdate update2;
            update2.hand = {
                Card(RANK_ACE, SUIT_SPADES),
                Card(RANK_KING, SUIT_DIAMONDS),
                Card(RANK_QUEEN, SUIT_HEARTS),
                Card(RANK_JACK, SUIT_CLUBS)
            };
            update2.middle_cards[CARDSLOT_1] = Card(RANK_ACE, SUIT_SPADES);
            update2.middle_cards[CARDSLOT_3] = Card(RANK_QUEEN, SUIT_SPADES);
            update2.middle_cards[CARDSLOT_5] = Card(RANK_JACK, SUIT_HEARTS);
            update2.middle_cards[CARDSLOT_6] = Card(RANK_KING, SUIT_DIAMONDS);
            update2.middle_cards[CARDSLOT_3_TOP] = Card(RANK_TEN, SUIT_CLUBS);
            update2.middle_cards[CARDSLOT_5_TOP] = Card(RANK_KING, SUIT_HEARTS);

            update2.draw_pile_cards = 1;
            update2.opponent_cards[8] = 3;
            update2.opponent_cards[7] = 16;
            update2.opponent_cards[10] = 5;
            update2.trump_card = Card();
            update2.trump_suit = SUIT_HEARTS;

            Network::sendMessage(std::make_unique<CardUpdate>(update2), id);

            BattleStateUpdate update4;
            update4.attackers = {8};
            update4.defender = 7;
            update4.idle = {10};
            Network::sendMessage(std::make_unique<BattleStateUpdate>(update4), id);
        }
    }

    return 0;
}
