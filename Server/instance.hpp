#pragma once

#include "protocol.hpp"
#include "game.hpp"

class Instance {
    public:

    //das chasch du alles modifiziere. mach das wi du willsch
    void broadcastState(Protocol::GameStage stage,
        uint draw_pile_cards,
        Game::Card trump_card, //wenn draw pile cards 0 isch rendered eifach nur de suit
        std::map<Protocol::CardSlot, Game::Card> middle_cards,
        std::map<Game::Player, uint> player_card_count,
        std::map<Game::Player, Protocol::PlayerRole> player_roles);

    void sendPrivateState(Game::Player player,     
        std::list<Protocol::GameAction> available_actions,
        std::list<Protocol::Card> cards);

    //TODO: timer das chömmer spöter mache
};