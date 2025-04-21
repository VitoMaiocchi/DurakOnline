#pragma once

#include "protocol.hpp"
#include <vector>
#include "game.hpp"


class CardManager{
    private:
        Protocol::Suit trump_suit_m;
        Protocol::Card trump_card_m;


    public:
        CardManager(Game::Player player_count);

        void determineTrump();
        void shuffleCards();

        void distributeNewCards();
};