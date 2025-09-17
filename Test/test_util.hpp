// tests/test_util.hpp
#pragma once
#include <gtest/gtest.h>
#include "../Server/gamehelper.hpp"
#include "../Server/helperheader.hpp"
#include "../Server/gamelogic.hpp"
#include "../Server/game.hpp"
#include <tuple>
#include <algorithm>

using namespace GameHelpers;
using namespace Protocol;

// compares two cards, the ranks and the suits, i think this is bullcrap and needs to be removed
static auto card_cmp = [](Card const &a, Card const &b) {
    return std::tie(a.rank, a.suit) < std::tie(b.rank, b.suit);
};


inline int findAttacker_TESTHELPER(State& s){
    auto attacker_it = std::find_if(s.player_roles.begin(), s.player_roles.end(), 
        [](PlayerRole role){return role == ATTACKER;});
    int attacker_idx = -1;
    if(attacker_it != s.player_roles.end()){
        attacker_idx = std::distance(s.player_roles.begin(), attacker_it);
    }
    return attacker_idx;
}

inline void setupHandsOfPlayers_TESTHELPER(State &s){
    const int n = s.player_count;
    if(n < 2 || n > 6){
        throw std::invalid_argument( " Wrong amount of players " );
    }

    //theres always at least an attacker and a defender, the others get distributed if the count is high enough
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;

    //clear all hands
    for(auto& hand : s.player_hands) hand.clear();

    //hand out cards, controlled, attacker and defender
    s.player_hands[attacker_idx].insert({RANK_TWO, SUIT_CLUBS});
    s.player_hands[defender_idx].insert({RANK_THREE, SUIT_CLUBS});

    //extra cards to be distributed among extra players if the count is bigger than 2
    const Protocol::Card extras[] = {
        {RANK_TWO,  SUIT_HEARTS},
        {RANK_FOUR, SUIT_HEARTS},
        {RANK_FIVE, SUIT_HEARTS},
        {RANK_SIX,  SUIT_HEARTS}
    };

    int idx = (defender_idx + 1) % n; //currently on coattacker
    for(int i = 0; i < n - 2; ++i){ //loop over extra players and give them cards
        s.player_hands[idx].insert(extras[i]);
        idx = (idx + 1) % n;
    }
}

inline void printRoles_TEST_HELPER(State &s){
    for(int i = 0; i < s.player_count; ++i){
        std::cout << "role: " << s.player_roles[i] << std::endl;
    }
}

inline void placeCardsInMiddleSlot_TESTHELPER(Card card, CardSlot slot, State &s){
    s.middle_cards[slot] = card;
    if(slot < 6) s.stage = GAMESTAGE_OPEN;

    auto cards = countCardsInMiddle(s);
    if(slot >= 6 && cards.second == 0) s.stage = GAMESTAGE_DEFEND;
}
