#include "card_manager.hpp"


CardManager::CardManager(GameLogic::Player player_count) {
    //initialize each player with an empty hand
    for (GameLogic::Player i = 0; i < player_count; ++i) {
        player_hands_m[i] = {};
    }

    fillDeck(); //initialize the deck with 52 cards

    shuffleCards(); //shufflecards

    distributeCardsBeginOfGame(); //distribute 6 cards to each player

    determineTrump();

    //card update?
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

void CardManager::shuffleCards(){
        // Check if deck has been initialized properly
        assert(deck_m.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
        // Define pseudo random number generator
        std::random_device rd;
        std::mt19937 g(rd());
        // shuffle the deck
        std::shuffle(deck_m.begin(), deck_m.end(), g);
}

void CardManager::distributeCardsBeginOfGame(){
    // distribute cards to player
    for (auto hand : player_hands_m){
        // Check if players hands are empty
        assert(hand.second.empty() && "Player's hand should be empty before dealing");

        // distribute the top 6 cards to the player
        hand.second.insert(hand.second.begin(), deck_m.begin(), deck_m.begin()+6);

        // remove cards from deck
        deck_m.erase(deck_m.begin(), deck_m.begin()+6);
    }
}

void CardManager::determineTrump(){
    assert(deck_m.size() == 52 - (6 * player_hands_m.size()) && "Deck must contain exactly 52 cards before determining trump");

    // Assign pointer
    last_card_m = std::make_shared<GameLogic::Card>(deck_m.back());

    trump_card_m = *last_card_m;
    trump_suit_m = trump_card_m.suit;
}