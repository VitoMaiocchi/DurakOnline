#ifndef GAME_HPP
#define GAME_HPP
#include "card_manager.hpp"
// #include "battle.hpp"

#include <Networking/util.hpp>

#include <vector>
#include <tuple>
#include <iostream>

using player_id = unsigned int;
class Battle; //forward declaration to avoid circular dependencies

class Game{

    private:
        std::vector<std::pair<int, PlayerRole>> players_bs; //attacking, defending, spectating
        // should this not be stored inside the battle class?
        // as the current role of the players is only relevant for the current battle
        Battle* current_battle;
        CardManager* card_manager;

    public:
        // constructor taking in an array of player ids
        Game(std::vector<player_id> player_ids);

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