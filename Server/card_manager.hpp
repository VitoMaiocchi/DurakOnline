#pragma once

#include "protocol.hpp"
#include "gamelogic.hpp"

#include <vector>
#include <deque>
class CardManager{
    private:
        GameLogic::Suit trump_suit_m;
        GameLogic::Card trump_card_m = 0; //default value

        std::deque<GameLogic::Card> deck_m;

    public:
        
        CardManager(GameLogic::Player player_count);

        void determineTrump();
        void shuffleCards();
        void fillDeck();

        void distributeNewCards();
};