#pragma once
#ifndef GAME_HPP
#define GAME_HPP

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
        std::vector<std::pair<int, PlayerRole>> players_;
        // pointer to the current battle
        Battle* current_battle_;
        // pointer to the card manager
        CardManager* card_manager_;

    public:
        // constructor taking in an array of player ids
        Game(std::vector<ClientID> player_ids);
        // destructor, should nominate the durak
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