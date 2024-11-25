#pragma once
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <Networking/message.hpp>

#include "../include/server.hpp"
#include "../include/game.hpp"

#include <unordered_set>
#include <memory>


struct Player{
    std::string name;
    ClientID player_id;
    bool durak = false;
};

namespace DurakServer{
    // Track connected clients
    extern std::unordered_set<ClientID> clients;

    // Track ready clients
    extern std::unordered_set<ClientID> ready_clients;

    // Game instance
    extern std::unique_ptr<Game> current_game;

    // Minimum number of players required to start a game
    constexpr size_t MIN_PLAYERS = 3;
    //maximum number of players that can play the game
    constexpr size_t MAX_PLAYERS = 6;
}

void broadcastMessage(std::unique_ptr<Message> message);

void handleMessage(std::unique_ptr<Message> msg_r, ClientID client);
