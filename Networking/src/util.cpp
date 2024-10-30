#include <Networking/util.hpp>

Card::Card() {
    rank = RANK_ACE;
    suit = SUIT_SPADES;
}

Card::Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}

Card::Card(uint i) {
    //TODO
}

const uint Card::toInt() const {
    //TODO
}

bool Card::operator==(const Card& other) const {
    //TODO
}

std::string Card::getFileName() {
    std::string s;
    std::string r;

    switch (suit) {
        case SUIT_CLUBS:
        s = "clubs";
        break;
        case SUIT_DIAMONDS:
        s = "diamonds";
        break;
        case SUIT_HEARTS:
        s = "hearts";
        break;
        case SUIT_SPADES:
        s = "spades";
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

    return std::string(CLIENT_RES_DIR) + "/cards/" + r + "_of_" + s + ".png";
}