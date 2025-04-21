#include "card_manager.hpp"



CardManager::CardManager(GameLogic::Player player_count) {
    fillDeck();
}


void CardManager::fillDeck(){

    using namespace Protocol;

    for(Suit suit : {SUIT_CLUBS,SUIT_SPADES,SUIT_DIAMONDS,SUIT_HEARTS}){
        for(Rank rank : {RANK_TWO,RANK_THREE,RANK_FOUR,RANK_FIVE,RANK_SIX,RANK_SEVEN,RANK_EIGHT,
                        RANK_NINE,RANK_TEN,RANK_JACK,RANK_QUEEN,RANK_KING,RANK_ACE}){
            deck_m.emplace_back(rank, suit);
        }
    }
    if(deck_m.size() != 52) {std::cerr << "Failed filling up the deck." << std::endl; return;}
}