#include "../include/msg_handler.hpp"
#include <iostream>

void handleMessage(std::unique_ptr<Message> message, ClientID client, Game* game /*, const std::unordered_set<ClientID>& clients*/){
    //da message handle dies das
    switch (message->messageType) {
        case MESSAGETYPE_TEST: {
            std::cout << "just a test message from client: " << client <<std::endl;
            // TestMessage* return_test = dynamic_cast<TestMessage*>(message.get());
            TestMessage* ret_test = dynamic_cast<TestMessage*>(message.get());
            std::cout << "string: " << ret_test->string
                      << "\nx: " <<ret_test->x
                      << "\ny: " <<ret_test->y << std::endl;
            dynamic_cast<TestMessage*>(message.get())->x = client;
            Network::sendMessage(message, client);
        }
        break;
        case MESSAGETYPE_PLAYCARD_EVENT: {
            //something
            // PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());
            /*get_playcard_msg(return_pce, client, current_game);*/
            //calls game function handleClientCardEvent();
            if(game != nullptr){
                game->handleClientCardEvent(std::move(message), client);
            }
        }
        break;
        case MESSAGETYPE_CLIENT_ACTION_EVENT:
            //something
        break;
        case MESSAGETYPE_CLIENT_CONNECT_EVENT:
            //something
        break;
        case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
            std::cout << "CLIENT DISCONNECTED: "<< client << std::endl;
            //if this happens the game should receive the message and tell the cardmanager to burn 
            //the players cards
        break;
        default:
            std::cout << "messagetype not found" << std::endl;
        break;
    }
}


void get_playcard_msg(PlayCardEvent* ret_msg, ClientID client, Game* current_game){
    //now we take the message and break it down into the corresponding components
    std::vector<Card> vector_of_cards;
    for(auto card : ret_msg->cards){
        vector_of_cards.push_back(card);
    }
    CardSlot slot = ret_msg->slot;
    //here we should call the Game function handleClientCardEvent()
    
}

// void send_test_msg(TestMessage* ret_test, ClientID client, Game* current_game){
//     Network::sendMessage();
// }
            // case MESSAGETYPE_TEST:
            //     dynamic_cast<TestMessage*>(m.get())->x = id;
            //     for(auto client : clients) {
            //         Network::sendMessage(m, client);
            //         std::cout << "message sending with type: " << m->messageType << std::endl;
            //     }
            // break;