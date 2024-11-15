#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "card_manager.hpp"
#include "../../Networking/include/Networking/util.hpp"

#include <vector>
#include <tuple>
#include <map>

using player_id = unsigned int;
class Battle; //forward declaration to avoid circular dependencies

class Game{

    private:
        // vector of pairs containing the player ids and their roles
        // has to be stored here because battle might be destructed
        std::map<ClientID, PlayerRole> player_roles_;
        // pointer to the current battle
        Battle* current_battle_;
        // pointer to the card manager
        CardManager* card_manager_;

    public:
        // constructor taking in an array of player ids
        Game(std::vector<ClientID> player_ids);
        // destructor, should nominate the durak
        ~Game();

        bool createBattle();
        bool isStarted();
        bool endGame();
        bool resetGame();
        bool updateTurnOrder();
        bool handleClientActionEvent();
        bool handleClientCardEvent();
};


#endif