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

namespace GameHelpers {
    
        void cardSetup(State &state){
            state.player_hands.resize(state.player_count); //initializes empty hands
        
            fillDeck(state);
            shuffleCards(state);
            distributeCardsBeginOfGame(state);
            determineTrump(state);
        }


        void doneEvent(State &state){/*TODO*/}

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