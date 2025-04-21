#pragma once

#include "protocol.hpp"
#include "gamelogic.hpp"

#include <vector>
#include <deque>
#include <algorithm>
#include <cassert>
#include <random>

class CardManager{
    private:
        GameLogic::Suit trump_suit_m;
        GameLogic::Card trump_card_m = 0; //default value

        std::deque<GameLogic::Card> deck_m;
        std::map<GameLogic::Player, std::vector<GameLogic::Card>> player_hands_m;

        std::shared_ptr<GameLogic::Card> last_card_m = std::make_shared<GameLogic::Card>();
        
    public:
        
        CardManager(GameLogic::Player player_count);

        void determineTrump();
        void shuffleCards();
        void fillDeck();

        void distributeCardsBeginOfGame();
        void distributeNewCards();
};