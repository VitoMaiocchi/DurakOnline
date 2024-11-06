#include "../include/card_manager.hpp"
#include <algorithm>
#include <cassert>
#include <random>
#include <iostream>


//constructors

cardManager::cardManager(unsigned int number_of_players){
    //Deck erstelle mit 52 charte TODO:

    //charte mische & usteile
    shuffleCards();

    //Trumpf bestimme
    determineTrump();

}

//At the beginning of the game

//Brucht die funktion en rückgabetyp?
//PRE: A deque containing all 52 cards in the deck (evtl au nöd?), empty player_hands vector
//POST: Returns true if the deck was succesfully shuffled, stored in the deck member and 6 cards were distributed to every palyer
void cardManager::shuffleCards(){
    // Check if deck has been initialized properly
    assert(deck.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define random number generator (Ich weiss nöd öb das de besti rng isch)
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(deck.begin(), deck.end(), g);

    
    //distribute cards to player
    for (unsigned int i=0; i < player_hands.size(); ++i){
        //Check if players hands are empty
        assert(player_hands[i].empty() && "Player's hand should be empty before dealing");

        //distribute the top 6 cards to the player
        player_hands[i].insert(player_hands[i].begin(), deck.begin(), deck.begin()+6);

        //remove cards from deck
        deck.erase(deck.begin(), deck.begin()+6);
    }

}

//PRE: A complete  shuffled deck containing the 52 cards
//POST: Sets the trump suit according to last card in the deck
Suit cardManager::determineTrump(){
    // Check if deck has been initialized properly
    assert(deck.size() == 52/*-6*Number Players*/ && "Deck must contain exactly 52 cards before determining trump");

    // Assign pointer
    last_card = std::make_shared<Card>(deck.back());

    // Check if pointer has been assigned
    assert(*last_card && "Pointer to last card is void")

    //Lueg mal öb ich de const mache chan
    return last_card->suit();
    
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//POST: A vector containing all the players cards sorted
std::vector<Card> cardManager::getPlayerHand (unsigned int PlayerID){
    assert(PlayerID >= 6);
    return player_hands.at(PlayerID);
}

//PRE:
//POST: Returns a vector of pairs returning all the cards in the middle
std::vector<std::pair<Card,Card>> cardManager::getMiddle(){
    return Middle;
}

//PRE:
//POST: Returns number of active players (players that haven't finished yet)
unsigned int cardManager::getNumberActivePlayers(){
    unsigned int activePlayers = std::count_if(player_hands.begin(), player_hands.end(), [](const std::vector<int>& v) {
        return v.empty()
    });
    return activePlayers;
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//PROST: Returns the number of cards that player currently has in his hands
unsigned int cardManager::getNumberOfCardsInHand(unsigned int PlayerID){
    return player_hands.at(PlayerID).size();
}

//PRE:A valid move 
//POST:Middle, Playerhand & numberofcards in hand updated
bool cardManager::attackCard(Card card, unsigned int PlayerID){
    //Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card) != player_hands[PlayerID].end() && "Card not found in the players hand");
    //save position of the card in the players hand
    auto cardPosition = std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card);
    

    //Position of next free slot in the middle should be passed to this function


    //Update middle number of cards in middle & in player hand
    ++number_cards_Middle;
    --player_number_of_cards[PlayerID];

    return 0;
}

//PRE: A valid move 
//POST: Middle, Playerhand & numberofcards in hand updated
void cardManager::defendCard(Card card, unsigned int PlayerID, unsigned int slot){
    //Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card) != player_hands[PlayerID].end() && "Card not found in the players hand");
    //save position of the card in the players hand
    auto cardPosition = std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card);
    //Place Card
    Middle[slot].second=card;
    //Remove card from the players hand
    player_hands[PlayerID].erase(cardPosition);
    
    //Update middle number of cards in middle & in player hand
    ++number_cards_Middle;
    --player_number_of_cards[PlayerID];
}

//PRE:
//POST: All cards in the middle are moved from "middle" to "discarded cards"
bool cardManager::clearMiddle(){
    //Mit de andere abmache wie das mir dmitti implementiered

    //Alli charte transfere vo mitti zu discarded

    //Überprüefe öb charte ufem dicarded+stack+playerhands=52 isch

    //Azahl charte i de mitti apasse
    return 0;
}   

//PRE:
//POST: All cards in the middle are assigned to the defenders hands
bool cardManager::pickUp(){
    //Danil söll mer nomal erkläre woe genau er drolle ispeicheret (agriife verteidige)
    return 0;
}   

//PRE:
//POST: Every player now has 6 or more cards in their hand except if the middle is empty
bool cardManager::distributeNewCards(){
    // Mer bruched irgendwie e agriffsreihefolg für das
    return 0;
}


//PRE: Any two distinct cards from the deck
//POST: returns true if card 2 is "greater" than card one in the sense of durak rules
bool cardManager::compareCards(Card card1, Card card2){
    if (card1.suit == trump){
        return card2.suit==trump && card2.rank > card1.rank;
    }
    return card2.suit==trump || (card2.suit==card1.suit && card2.rank > card1.rank);
}