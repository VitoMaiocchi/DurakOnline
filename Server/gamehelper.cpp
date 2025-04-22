#include "gamehelper.hpp"


void fillDeck(State &state){
    using namespace Protocol;
    
    for(Suit suit : {SUIT_CLUBS,SUIT_SPADES,SUIT_DIAMONDS,SUIT_HEARTS}){
        for(Rank rank : {RANK_TWO,RANK_THREE,RANK_FOUR,RANK_FIVE,RANK_SIX,RANK_SEVEN,RANK_EIGHT,
            RANK_NINE,RANK_TEN,RANK_JACK,RANK_QUEEN,RANK_KING,RANK_ACE}){
                state.draw_pile.emplace_back(rank, suit);
        }
    }
}
    
void shuffleCards(State &state){
    // Check if deck has been initialized properly
    assert(state.draw_pile.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define pseudo random number generator
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(state.draw_pile.begin(), state.draw_pile.end(), g);
}
    
void distributeCardsBeginOfGame(State &state){
    // distribute cards to player
    for (auto& hand : state.player_hands){
        // Check if players hands are empty
        assert(hand.empty() && "Player's hand should be empty before dealing");
        
        // distribute the top 6 cards to the player
        hand.insert(state.draw_pile.begin(), state.draw_pile.begin()+6);
        
        // remove cards from deck
        state.draw_pile.erase(state.draw_pile.begin(), state.draw_pile.begin()+6);
    }
}
    
void determineTrump(State &state){
    assert(state.draw_pile.size() == 52 - (6 * state.player_hands.size()) && "Deck must contain exactly 52 cards before determining trump");
    
    state.trump_card = state.draw_pile.back();
}   

void clearMiddle(State &state){
    for(auto &opt : state.middle_cards){
        opt.reset();
    }
}

void drawFromMiddle(Player player){

}

void distributeNewCards(State &state){
    using namespace Protocol;
    std::vector<PlayerRole> role_seq;
    role_seq.reserve(state.player_count);
    role_seq.push_back(ATTACKER);
    role_seq.push_back(CO_ATTACKER);
    role_seq.insert(role_seq.end(), state.player_count - 3, IDLE);
    role_seq.push_back(DEFENDER);

    // for(int i = 0; i < state.player_count; ++i){
    //     std::find_if(state.player_roles.begin(), state.player_roles.end(), 
    //                 [](){});
    //     drawFromMiddle(i);
    // }
}
void deleteOldBattle(State &state){
    //clear middle
    clearMiddle(state);
    //distribute cards
    distributeNewCards(state);

}
namespace GameHelpers {
    
        void cardSetup(State &state){
            state.player_hands.resize(state.player_count); //initializes empty hands
        
            fillDeck(state);
            shuffleCards(state);
            distributeCardsBeginOfGame(state);
            determineTrump(state);
        }

        //attacker or coattacker can trigger
        void doneEvent(Player player, State &state){
            using namespace Protocol;
            if(state.player_roles[player] == ATTACKER) state.ok_msg[ATTACKER] = true;
            if(state.player_roles[player] == CO_ATTACKER) state.ok_msg[CO_ATTACKER] = true;

            switch(state.stage){
                case GAMESTAGE_DEFEND : {
                    if(BATTLETYPE_ENDGAME) state.ok_msg[CO_ATTACKER] = true;
                    //clearmiddle
                    //deleteoldbattle
                    deleteOldBattle(state);
                    //startnewbattle
                }
            }
            
        }

        //only defender can trigger
        void reflectEvent(State &state){/*TODO*/}
        void pickUpEvent(State &state){/*TODO*/}

        void updateAvailableActions(State &state){
            switch(state.stage){
                case Protocol::GameStage::GAMESTAGE_FIRST_ATTACK : {
                    /*no available actions*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_OPEN : {
                    /*pick up and/or reflect for defender*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_DEFEND : {
                    /*ready for both attacker*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_POST_PICKUP : {
                    /*ready for both attackers*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_DONE : {
                    /*no available actions, destroy battle, setup new battle*/
                    break;
                }
            }
        }
}