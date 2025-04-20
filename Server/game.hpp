#pragma once

#include "protocol.hpp"

class Instance;

class Game {
    private:
    Instance* parent_instance;

    public:
    //wenn player action nöd guet isch den eifach es eigenes mache nöd protocol modifiziere
    typedef Protocol::GameAction PlayerAction;
    typedef Protocol::Card Card;
    typedef uint Player;

    //die signatures chasch au mache wie du willsch
    Game(Player player_count, Instance* parent_instance);
    void playerActionNotify(Player player, PlayerAction action);
    void playerCardNotify(Player player, uint slot, std::list<Card> card);
    void disconnectNotify(Player player); //unexpected disconnect mid game;
};