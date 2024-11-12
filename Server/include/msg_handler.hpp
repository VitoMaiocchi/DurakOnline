#ifndef MSG_HANDLER_HPP
#define MSG_HANDLER_HPP

#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"
// #include "../include/battle.hpp"

#include <unordered_set>
#include <memory>

void handleMessage(std::unique_ptr<Message> message, ClientID client /*,const std::unordered_set<ClientID>& clients, Game* current_game*/);

void get_playcard_msg(PlayCardEvent* ret_msg, ClientID client, Game* current_game);
#endif