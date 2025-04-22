#pragma once

#include "protocol.hpp"
#include <vector>
#include <unordered_set>
#include <deque>

namespace std {
    template <>
    struct hash<Protocol::Card> {
        size_t operator()(const Protocol::Card& card) const {
            return hash<int>()(static_cast<int>(card.suit) * Protocol::RANK_count + static_cast<int>(card.rank));
        }
    };
}

namespace GameLogic {
    using Player = uint;
    using PlayerAction = Protocol::GameAction;
    using Card = Protocol::Card;
    using Suit = Protocol::Suit;
    using Rank = Protocol::Rank;

    typedef std::vector<std::unordered_set<Card>> PlayerHands;
    typedef std::vector<std::unordered_set<PlayerAction>> AvailableActions;

    struct State {
        Player player_count;
        Protocol::GameStage stage;
        std::deque<GameLogic::Card> draw_pile;
        std::map<Protocol::CardSlot, Card> middle_cards;
        std::vector<Protocol::PlayerRole> player_roles;
        PlayerHands player_hands;
        AvailableActions available_actions;
        Card trump_card;
        BattleType battle_type;

        //ctor to initialize default values for State
        State(Player count) : player_count(count), 
                                    trump_card(0){ //default initialization for trump_card
            player_roles.resize(count);
            player_hands.resize(count);
            available_actions.resize(count);
        }
    };

    enum BattleType { 
        BATTLETYPE_FIRST,  // First battle of the game
        BATTLETYPE_NORMAL, // Normal battle
        BATTLETYPE_ENDGAME // Endgame battle (only two players left)
    };

}