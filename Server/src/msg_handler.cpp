#include "../include/msg_handler.hpp"
#include <iostream>

void handleMessage(std::unique_ptr<Message> message, ClientID client /*, const std::unordered_set<ClientID>& clients, Game* current_game*/){
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
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            //something
        break;
        case MESSAGETYPE_CARD_UPDATE:
            //do something
            //Network::sendMessage(client, goo goo gagag);
        break;
        case MESSAGETYPE_PLAYER_UPDATE:
            //something
        break;
        case MESSAGETYPE_BATTLE_STATE_UPDATE:
            //something
            // Network::sendMessage(message, client);
            //has to send a message from either game or from battle to the client
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            //something
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            //something
        break;
        case MESSAGETYPE_PLAYCARD_EVENT: {
            //something
            PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());
            /*get_playcard_msg(return_pce, client, current_game);*/
            //calls game function handleClientCardEvent();
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