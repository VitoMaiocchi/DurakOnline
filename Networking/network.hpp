#pragma once

#include <string>
#include <memory>
#include "message.hpp"
#define uint unsigned int
#define ClientID uint

namespace Network {
    //SERVER USE
    #ifdef NETWORKTYPE_SERVER
    void openSocket(uint port);
    void sendMessage(std::unique_ptr<Message> &message, ClientID id);
    std::unique_ptr<Message> reciveMessage(ClientID &id);
    #endif

    //CLIENT USE
    #ifdef NETWORKTYPE_CLIENT
    void openConnection(std::string ip, uint port);
    void sendMessage(std::unique_ptr<Message> &message);
    std::unique_ptr<Message> reciveMessage();
    #endif
}