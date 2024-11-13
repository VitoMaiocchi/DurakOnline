#pragma once
#ifndef GAME_HPP
#define GAME_HPP
// #include "battle.hpp"

#include "card_manager.hpp"
#include "../../Networking/include/Networking/util.hpp"

#include <vector>
#include <tuple>

using player_id = unsigned int;
class Battle; //forward declaration to avoid circular dependencies

class Game{

    private:
        // vector of pairs containing the player ids and their roles
        // has to be stored here because battle might be destructed
        std::vector<std::pair<int, PlayerRole>> players_bs;
        Battle* current_battle;
        CardManager* card_manager;

    public:
        // constructor taking in an array of player ids
        Game(std::vector<ClientID> player_ids);
        // destructor taking in Client ID of Durak to be able to send endgame message
        ~Game();

        bool makeFirstBattle();
        bool createBattle();
        bool isStarted();
        bool endGame();
        bool resetGame();
        bool updateTurnOrder();
        bool handleClientActionEvent();
        bool handleClientCardEvent();
};


#endif