#pragma once

#include "../protocol.hpp"
#include <vector>
#include <unordered_set>
#include <deque>
#include <array>
#include <algorithm>
#include <optional>


namespace std {
    template <>
    struct hash<Protocol::Card> {
        size_t operator()(const Protocol::Card& card) const {
            return hash<int>()(static_cast<int>(card.suit) * Protocol::RANK_count + static_cast<int>(card.rank));
        }
    };
}

namespace GameLogic {
    using Player = int;
    using PlayerAction = Protocol::GameAction;
    using Card = Protocol::Card;
    using Suit = Protocol::Suit;
    using Rank = Protocol::Rank;

    typedef std::vector<std::unordered_set<Card>> PlayerHands;
    typedef std::vector<std::unordered_set<PlayerAction>> AvailableActions;
    
    enum BattleType { 
        BATTLETYPE_FIRST,  // First battle of the game
        BATTLETYPE_NORMAL, // Normal battle
        BATTLETYPE_ENDGAME // Endgame battle (no more cards in the deck)
    };

    
    struct State {
        Player player_count;
        Protocol::GameStage stage;
        std::deque<GameLogic::Card> draw_pile;
        std::array<std::optional<Card>, Protocol::CARDSLOT_COUNT> middle_cards;
        std::vector<Protocol::PlayerRole> player_roles;
        PlayerHands player_hands;
        AvailableActions available_actions;
        Card trump_card = 0;//default
        BattleType battle_type;
        // need two ok messages, one from attacker and one from coattacker
        std::map<Protocol::PlayerRole, bool> ok_msg;
        Player durak;

    };


}