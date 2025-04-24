#include <gtest/gtest.h>
#include "../Server/gamehelper.hpp"
#include "../Server/helperheader.hpp"
#include "../Server/gamelogic.hpp"
#include <tuple>
#include <algorithm>

using namespace GameHelpers;
using namespace Protocol;


static auto card_cmp = [](Card const &a, Card const &b) {
    return std::tie(a.rank, a.suit) < std::tie(b.rank, b.suit);
};

TEST(FillDeck, Has52Cards) {
    State s{4};
    fillDeck(s);
    EXPECT_EQ(s.draw_pile.size(), 52);
}

TEST(Shuffle, CardsAreDifferentOrder) {
    State s{4};
    fillDeck(s);
    auto before = s.draw_pile;
    shuffleCards(s);
    auto after = s.draw_pile;

    EXPECT_NE(before, after);
    EXPECT_EQ(before.size(), after.size());
}

TEST(DistributeCardsBegin, EveryPlayerHas6cards){
    State s{4};
    fillDeck(s);
    shuffleCards(s);
    distributeCardsBeginOfGame(s);

    EXPECT_EQ(6, s.player_hands[0].size());
    EXPECT_EQ(6, s.player_hands[1].size());
    EXPECT_EQ(6, s.player_hands[2].size());
    EXPECT_EQ(6, s.player_hands[3].size());
}

TEST(DetermineTrump, CheckThatATrumpWasGiven){
    State s{4};
    fillDeck(s);
    shuffleCards(s);
    distributeCardsBeginOfGame(s);
    auto trump_suit = SUIT_count;
    auto trump_rank = RANK_count;
    determineTrump(s);
    trump_suit = s.trump_card.suit;
    trump_rank = s.trump_card.rank;

    EXPECT_NE(SUIT_count, trump_suit);
    EXPECT_NE(RANK_count, trump_rank);

}



TEST(PlayerRoles, MoveTheRoles){
    State s{4};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);
    auto attacker_it = std::find_if(s.player_roles.begin(), s.player_roles.end(), 
        [](PlayerRole role){return role == ATTACKER;});
    int attacker_idx = -1;
    if(attacker_it != s.player_roles.end()){
        attacker_idx = std::distance(s.player_roles.begin(), attacker_it);
    }
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int idle_idx = (coattacker_idx + 1) % s.player_count;

    movePlayerRoles(s);

    EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
    EXPECT_EQ(s.player_roles[defender_idx], ATTACKER);
    EXPECT_EQ(s.player_roles[coattacker_idx], DEFENDER);
    EXPECT_EQ(s.player_roles[idle_idx], CO_ATTACKER);
}