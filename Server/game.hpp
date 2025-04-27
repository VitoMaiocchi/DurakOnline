#pragma once

#include "protocol.hpp"
#include "gamelogic.hpp"
#include "gamehelper.hpp"
#include <vector>
#include <ranges>
#include <algorithm>

class Instance;

class Game {
    private:
        Instance* parent_instance_m;
        GameLogic::State game_state_m;

    public:
        //die signatures chasch au mache wie du willsch
        Game(GameLogic::Player player_count, Instance* parent_instance_m, Durak previous_durak);
        void handlePlayerAction(GameLogic::Player player, GameLogic::PlayerAction action);
        void playerCardEvent(GameLogic::Player player, uint slot, std::list<GameLogic::Card> card);
        void disconnectNotify(GameLogic::Player player); //unexpected disconnect mid game;
};