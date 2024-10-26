#pragma once
#include <string>
#include <exception>
#include <iostream>

#define CLIENT_RES_DIR "../Client/resources"

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

#define PRINT_DEBUG(msg) \
    std::cout << "DEBUG: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl

#define THROW_ERROR(msg) \
    throw std::runtime_error(std::string(msg) + " (" + __FILE__ + ":" + std::to_string(__LINE__) + ")")
