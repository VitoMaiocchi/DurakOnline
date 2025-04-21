#pragma once

#include "protocol.hpp"
#include "card_manager.hpp"
#include <vector>

class Instance;

class Game {
    private:
        Instance* parent_instance_m;
        std::vector<Protocol::PlayerRole> player_roles_m;
        CardManager card_manager_m;

    public:
        //die signatures chasch au mache wie du willsch
        Game(GameLogic::Player player_count, Instance* parent_instance_m);
        void playerActionNotify(GameLogic::Player player, GameLogic::PlayerAction action);
        void playerCardNotify(GameLogic::Player player, uint slot, std::list<GameLogic::Card> card);
        void disconnectNotify(GameLogic::Player player); //unexpected disconnect mid game;
        
        void findFirstAttacker();
};