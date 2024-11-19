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

void handleMessage(std::unique_ptr<Message> msg_r, ClientID client, std::unique_ptr<Game>& current_game, std::unordered_set<ClientID> clients, std::unordered_set<ClientID>& ready_clients);

#endif