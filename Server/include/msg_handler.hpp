#ifndef MSG_HANDLER_HPP
#define MSG_HANDLER_HPP

#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"


void handleMessage(std::unique_ptr<Message> message, ClientID client);
#endif