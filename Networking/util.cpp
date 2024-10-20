#include "util.hpp"

Card::Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}

std::string Card::getFileName() {
    std::string s;
    std::string r;

    switch (suit) {
        case SUIT_CLUBS:
        r = "clubs";
        break;
        case SUIT_DIAMONDS:
        r = "diamonds";
        break;
        case SUIT_HEARTS:
        r = "hearts";
        break;
        case SUIT_SPADES:
        r = "spades";
        break;
    } 

    switch (rank) {
        case RANK_TWO:
        r = "2";
        break;
        case RANK_THREE:
        r = "3";
        break;
        case RANK_FOUR:
        r = "4";
        break;
        case RANK_FIVE:
        r = "5";
        break;
        case RANK_SIX:
        r = "6";
        break;
        case RANK_SEVEN:
        r = "7";
        break;
        case RANK_EIGHT:
        r = "8";
        break;
        case RANK_NINE:
        r = "9";
        break;
        case RANK_TEN:
        r = "10";
        break;
        case RANK_JACK:
        r = "jack";
        break;
        case RANK_QUEEN:
        r = "queen";
        break;
        case RANK_KING:
        r = "king";
        break;
        case RANK_ACE:
        r = "ace";
        break;
    }

    return r + "_of_" + s + ".png";
}