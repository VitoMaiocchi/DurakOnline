#include "../include/msg_handler.hpp"
#include <iostream>

void handleMessage(std::unique_ptr<Message> message, ClientID client){
    //da message handle dies das
    switch (message->messageType) {
        case MESSAGETYPE_TEST:
            std::cout << "just a test message"<<std::endl;
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
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            //something
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            //something
        break;
        case MESSAGETYPE_PLAYCARD_EVENT:
            //something
        break;
        case MESSAGETYPE_CLIENT_ACTION_EVENT:
            //something
        break;
        case MESSAGETYPE_CLIENT_CONNECT_EVENT:
            //something
        break;
        case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
            std::cout << "CLIENT DISCONNECTED: "<< client << std::endl;
            //if this happens the game should receibe the message and tell the cardmanager to burn the plazers cards
        break;
        default:
            std::cout << "messagetype not found" << std::endl;
        break;
    }
}