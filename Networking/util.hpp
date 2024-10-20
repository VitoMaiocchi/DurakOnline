#pragma once
#include <string>

enum Suit {
    SUIT_CLUBS,
    SUIT_SPADES,
    SUIT_DIAMONDS,
    SUIT_HEARTS
};

enum Rank {
    RANK_TWO,
    RANK_THREE,
    RANK_FOUR,
    RANK_FIVE,
    RANK_SIX,
    RANK_SEVEN,
    RANK_EIGHT,
    RANK_NINE,
    RANK_TEN,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE
};

struct Card {
    Suit suit;
    Rank rank;

    Card(Rank rank, Suit suit);
    std::string getFileName();
};