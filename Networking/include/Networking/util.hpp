#pragma once
#include <string>
#include <exception>
#include <iostream>

#define CLIENT_RES_DIR std::string("../Client/resources/")

typedef unsigned int uint;
typedef uint ClientID;

enum Suit {
    SUIT_CLUBS,
    SUIT_SPADES,
    SUIT_DIAMONDS,
    SUIT_HEARTS,
    SUIT_count,
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
    RANK_ACE,
    RANK_count,
};

struct Card {
    Suit suit;
    Rank rank;

    Card();
    Card(Rank rank, Suit suit);
    Card(uint i);
    std::string getFileName();
    const uint toInt() const;

    bool operator==(const Card& other) const;

    bool operator!=(const Card& other) const;
};

namespace std {
    template <>
    struct hash<Card> {
        size_t operator()(const Card& card) const {
            return hash<int>()(static_cast<int>(card.suit) * RANK_count + static_cast<int>(card.rank));
        }
    };
}

enum CardSlot {
    CARDSLOT_1,
    CARDSLOT_2,
    CARDSLOT_3,
    CARDSLOT_4,
    CARDSLOT_5,
    CARDSLOT_6,
    CARDSLOT_1_TOP, //für karte wo uf de andere ligged
    CARDSLOT_2_TOP,
    CARDSLOT_3_TOP,
    CARDSLOT_4_TOP,
    CARDSLOT_5_TOP,
    CARDSLOT_6_TOP,
    CARDSLOT_NONE
};

enum GameState {
    GAMESTATE_LOBBY,
    GAMESTATE_GAME,
    GAMESTATE_SPECTATOR,
    GAMESTATE_GAME_OVER,
    GAMESTATE_DURAK_SCREEN,
    GAMESTATE_LOGIN_SCREEN //only for client use
};
// inline int ToInt(GameState state) { return static_cast<int>(state); }
// inline GameState FromInt(int value) { return static_cast<GameState>(value); }

enum ClientAction {
    CLIENTACTION_OK,
    CLIENTACTION_PASS_ON,
    CLIENTACTION_PICK_UP,
    CLIENTACTION_READY,
    CLIENTACTION_LOBBY
};

enum PlayerRole {
    ATTACKER,
    DEFENDER,
    CO_ATTACKER,
    IDLE,
    FINISHED
}; //max 6 players

#define PRINT_DEBUG(msg) \
    std::cout << "DEBUG: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl

#define THROW_ERROR(msg) \
    throw std::runtime_error(std::string(msg) + " (" + __FILE__ + ":" + std::to_string(__LINE__) + ")")


// generic function to do enums to ints
template<typename T>
inline int ToInt(T value) {return static_cast<int>(value); }
// generic function to do ints back to the correct enum
template<typename T>
inline T FromInt(int value) { return static_cast<T>(value); }