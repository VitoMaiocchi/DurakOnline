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
    State s{2};
    fillDeck(s);
    EXPECT_EQ(s.draw_pile.size(), 52);
}

TEST(Shuffle, CardsAreDifferentOrder) {
    State s{2};
    fillDeck(s);
    auto before = s.draw_pile;
    shuffleCards(s);
    auto after = s.draw_pile;

    EXPECT_NE(before, after);
    EXPECT_EQ(before.size(), after.size());
}