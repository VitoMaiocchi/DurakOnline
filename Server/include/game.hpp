#ifndef GAME_HPP
#define GAME_HPP
#include "card_manager.hpp"
#include "battle.hpp"

#include <Networking/util.hpp>

#include <vector>
#include <tuple>

using player_id = unsigned int;

class Game{
    private:
        std::vector<std::pair<int, PlayerRole>> players_bs; //attacking, defending, spectating
        Battle* current_battle;
        CardManager card_manager;
    public:
        bool createGame();
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