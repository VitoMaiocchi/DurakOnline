#pragma once

#include "protocol.hpp"
#include "gamelogic.hpp"

#include <vector>
#include <deque>
class CardManager{
    private:
        Protocol::Suit trump_suit_m;
        Protocol::Card trump_card_m = 0; //default value

        std::deque<Protocol::Card> deck_m;

    public:
        typedef Protocol::Card Card;
        

        CardManager(GameLogic::Player player_count);

        void determineTrump();
        void shuffleCards();
        void fillDeck();

        void distributeNewCards();
};