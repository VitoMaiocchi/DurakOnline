#include "card_manager.hpp"
#include <algorithm>
#include <cassert>
#include <random>


//constructors

//At the beginning of the game
//PRE: A deque containing all 52 cards in the deck (evtl au nöd?)
//POST: Returns true if the deck was succesfully shuffled, stored in the deck member and 6 cards were distributed to every palyer
bool card_manager::shuffleCards(){
    // Check if deck has been initialized properly
    assert(deck.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define random number generator (Ich weiss nöd öb das de besti rng isch)
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(deck.begin(), deck.end(),g);
    return 0;
}

//PRE: A complete  shuffled deck containing the 52 cards
//POST: Sets the trump suit according to last card in the deck
bool card_manager::determineTrump(){
    // Check if deck has been initialized properly
    assert(deck.size() == 52/*-6*Number Players*/ && "Deck must contain exactly 52 cards before determining trump");

    last_card = std::make_shared<Card>(deck.back());
    return 1;
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//POST: A vector containing all the players cards sorted
std::vector<Card> card_manager::getPlayerHand (unsigned int PlayerID){
    assert(PlayerID >= 6);
    return player_hands.at(PlayerID);
}

//PRE:
//POST: Returns a vector of pairs returning all the cards in the middle
std::vector<std::pair<Card,Card>> card_manager::getMiddle(){
    return Middle;
}

//PRE:
//POST: Returns number of active players (players that haven't finished yet)
unsigned int card_manager::getNumberActivePlayers(){
    return 0;
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//PROST: Returns the number of cards that player currently has in his hands
unsigned int card_manager::getNumberOfCardsInHand(unsigned int PlayerID){
    return player_hands.at(PlayerID).size();
}

//PRE:???
//POST:???
bool card_manager::playCard(Card card, unsigned int PlayerID){
    return 0;
}

//PRE:
//POST: All cards in the middle are moved from "middle" to "discarded cards"
bool card_manager::clearMiddle(){
    //Mit de andere abmache wie das mir dmitti implementiered
    return 0;
}   

//PRE:
//POST: All cards in the middle are assigned to the defenders hands
bool card_manager::pickUp(){
    //Danil söll mer nomal erkläre woe genau er drolle ispeicheret (agriife verteidige)
    return 0;
}   

//PRE:
//POST: Every player now has 6 or more cards in their hand except if the middle is empty
bool card_manager::distributeNewCards(){
    return 0;
}

//Bis jetzt mit rank als unsigned int, chömmer aber no ändere
//PRE: Any two distinct cards from the deck
//POST: returns true if card 2 is "greater" than card one in the sense of durak rules
bool card_manager::compareCards(Card card1, Card card2){
    if (card1.suit == trump){
        return card2.suit==trump && card2.rank > card1.rank;
    }
    return card2.suit==trump || (card2.suit==card1.suit && card2.rank > card1.rank);
}