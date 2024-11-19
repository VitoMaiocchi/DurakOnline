#pragma once

#include <string>
#include <memory>
#include <Networking/message.hpp>

typedef unsigned int uint;
typedef uint ClientID;

namespace Network {
    //SERVER USE
    #ifdef NETWORKTYPE_SERVER
    void openSocket(uint port);
    bool sendMessage(std::unique_ptr<Message> message, ClientID id); //returns false if client id not valid or disconnected
    std::unique_ptr<Message> reciveMessage(ClientID &id);
    #endif

    //CLIENT USE
    #ifdef NETWORKTYPE_CLIENT
    ClientID openConnection(std::string ip, uint port);
    void sendMessage(std::unique_ptr<Message> message);
    std::unique_ptr<Message> reciveMessage();
    #endif
}