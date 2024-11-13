#include <algorithm>
#include <cassert>
#include <random>
#include <iostream>
#include "../include/card_manager.hpp"


//constructors

CardManager::CardManager(std::vector<ClientID> player_ids){
    //Deck erstelle mit 52 charte TODO:
    fillDeck();
    //charte mische & usteile
    shuffleCards();

    //Trumpf bestimme
    determineTrump();

}

//dtor
CardManager::~CardManager() = default;
//At the beginning of the game

//Brucht die funktion en rückgabetyp?
//PRE: A deque containing all 52 cards in the deck (evtl au nöd?), empty player_hands vector
//POST: Returns true if the deck was succesfully shuffled, stored in the deck member and 6 cards were distributed to every palyer
void CardManager::shuffleCards(){
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
void CardManager::determineTrump(){
    // Check if deck has been initialized properly
    assert(deck.size() == 52/*-6*Number Players*/ && "Deck must contain exactly 52 cards before determining trump");

    // Assign pointer
    last_card = std::make_shared<Card>(deck.back());

    // Check if pointer has been assigned
    assert(last_card && "Pointer to last card is void");

    //Lueg mal öb ich de const mache chan
    trump = last_card->suit;
    
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//POST: A vector containing all the players cards sorted
std::vector<Card> CardManager::getPlayerHand (unsigned int PlayerID){
    assert(PlayerID >= 6);
    return player_hands.at(PlayerID);
}

//PRE:
//POST: Returns a vector of pairs returning all the cards in the middle
std::vector<std::pair<Card,Card>> CardManager::getMiddle(){
    return Middle;
}

//PRE:
//POST: Returns number of active players (players that haven't finished yet)
unsigned int CardManager::getNumberActivePlayers(){
    unsigned int activePlayers = std::count_if(player_hands.begin(), player_hands.end(), [](std::vector<Card>& v) {
        return v.empty();
    });
    return activePlayers;
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//PROST: Returns the number of cards that player currently has in his hands
unsigned int CardManager::getNumberOfCardsInHand(unsigned int PlayerID){
    return player_hands.at(PlayerID).size();
}


//PRE:A valid move 
//POST:Middle, Playerhand & numberofcards in hand updated
bool CardManager::attackCard(Card card, unsigned int PlayerID){
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
void CardManager::defendCard(Card card, unsigned int PlayerID, unsigned int slot){
    //Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card) != player_hands[PlayerID].end() && "Card not found in the players hand");
    //save position of the card in the players hand
    auto cardPosition = std::find(player_hands[PlayerID].begin(), player_hands[PlayerID].end(), card);
    //Place Card
    Middle[slot].second=card;
    //Remove card from the players hand
    player_hands[PlayerID].erase(cardPosition);
    
    //Update number of cards in middle & in player hand
    ++number_cards_Middle;
    --player_number_of_cards[PlayerID];
}

//PRE:
//POST: All cards in the middle are moved from "middle" to "discarded cards"
bool CardManager::clearMiddle(){
    assert(Middle.size()<=6 && "Middle shouldn't have more than six slots");
    //Alli charte transfere vo mitti zu discarded, bin nonig so zfriede mit dere implementation
    while(!Middle.empty()){
        discarded_cards.push_back(Middle[0].first);
        discarded_cards.push_back(Middle[0].second);
        Middle.erase(Middle.begin());
    }

    //Azahl charte i de mitti apasse
    number_cards_Middle=Middle.size();
    return 0;
}   

//PRE:
//POST: All cards in the middle are assigned to the defenders hands
void CardManager::pickUp(unsigned int PlayerID_def){
    //Danil söll ID vom defender mitgeh
    assert(Middle.size()<=6 && "Middle shouldn't have more than six slots");
    //Alli charte transfere vo mitti zu discarded, bin nonig so zfriede mit dere implementation
    while(!Middle.empty()){
        player_hands[PlayerID_def].push_back(Middle[0].first);
        discarded_cards.push_back(Middle[0].second);
        Middle.erase(Middle.begin());
    }
}   

//PRE:
//POST: Every player now has 6 or more cards in their hand except if the middle is empty
bool CardManager::distributeNewCards(){
    // Mer bruched irgendwie e agriffsreihefolg für das
    return 0;
}


//PRE: Any two distinct cards from the deck
//POST: returns true if card 2 is "greater" than card one in the sense of durak rules
bool CardManager::compareCards(Card card1, Card card2){
    if (card1.suit == trump){
        return card2.suit==trump && card2.rank > card1.rank;
    }
    return card2.suit==trump || (card2.suit==card1.suit && card2.rank > card1.rank);
}

Suit CardManager::getTrump(){
    return trump;
}


void CardManager::fillDeck() {
    for (int suit = SUIT_CLUBS; suit <= SUIT_HEARTS; ++suit) {  // Iterate over all suits
        for (int rank = RANK_TWO; rank <= RANK_ACE; ++rank) {    // Iterate over all ranks
            deck.emplace_back(static_cast<Rank>(rank), static_cast<Suit>(suit));
        }
    }
}
