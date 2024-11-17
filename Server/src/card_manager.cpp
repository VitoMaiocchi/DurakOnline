#include <algorithm>
#include <cassert>
#include <random>
#include <iostream>
#include "../include/card_manager.hpp"


//constructors

CardManager::CardManager(std::vector<ClientID> player_ids){
    //Deck erstelle mit 52 charte TODO:
    fillDeck();
    //adjust the size of the player hands
    player_hands_ = std::vector<std::vector<Card>>(player_ids.size());
    //charte mische & usteile
    shuffleCards();

    //Trumpf bestimme
    determineTrump();

    // at the end of the constructor card manager should communicate the current status of the cards in play
    // for this we use the message CARD_UPDATE
}

//dtor
CardManager::~CardManager() = default;
//At the beginning of the game

//Brucht die funktion en rückgabetyp?
//PRE: A deque containing all 52 cards in the deck_ (evtl au nöd?), empty player_hands vector
//POST: Returns true if the deck was succesfully shuffled, stored in the deck member and 6 cards were distributed to every palyer
void CardManager::shuffleCards(){
    // Check if deck has been initialized properly
    assert(deck_.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define random number generator (Ich weiss nöd öb das de besti rng isch)
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(deck_.begin(), deck_.end(), g);

    
    //distribute cards to player
    for (unsigned int i=0; i < player_hands_.size(); ++i){
        //Check if players hands are empty
        assert(player_hands_[i].empty() && "Player's hand should be empty before dealing");

        //distribute the top 6 cards to the player
        player_hands_[i].insert(player_hands_[i].begin(), deck_.begin(), deck_.begin()+6);

        //remove cards from deck
        deck_.erase(deck_.begin(), deck_.begin()+6);
    }

}

//PRE: A complete  shuffled deck containing the 52 cards
//POST: Sets the trump suit according to last card in the deck
void CardManager::determineTrump(){
    // Check if deck has been initialized properly
    assert(deck_.size() == 52 - (6 * player_hands_.size()) && "Deck must contain exactly 52 cards before determining trump");

    // Assign pointer
    last_card_ = std::make_shared<Card>(deck_.back());

    // Check if pointer has been assigned
    assert(last_card_ && "Pointer to last card is void");

    //Lueg mal öb ich de const mache chan
    trump_ = last_card_->suit;
    
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//POST: A vector containing all the players cards sorted
std::vector<Card> CardManager::getPlayerHand (unsigned int PlayerID){
    assert(PlayerID >= 6);
    return player_hands_.at(PlayerID);
}

//PRE:
//POST: Returns a vector of pairs returning all the cards in the middle
std::vector<std::pair<Card,Card> > CardManager::getMiddle(){
    return middle_;
}

//PRE:
//POST: Returns number of active players (players that haven't finished yet)
unsigned int CardManager::getNumberActivePlayers(){
    unsigned int activePlayers = std::count_if(player_hands_.begin(), player_hands_.end(), [](std::vector<Card>& v) {
        return v.empty();
    });
    return activePlayers;
}

//PRE: A valid PlayerID (unsigned int between 0 and #players-1)
//PROST: Returns the number of cards that player currently has in his hands
unsigned int CardManager::getNumberOfCardsInHand(unsigned int PlayerID){
    return player_hands_.at(PlayerID).size();
}


//PRE:A valid move 
//POST:Middle, Playerhand & numberofcards in hand updated
bool CardManager::attackCard(Card card, unsigned int PlayerID){
    //Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card) != player_hands_[PlayerID].end() && "Card not found in the players hand");
    //save position of the card in the players hand
    auto cardPosition = std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card);
    

    //Position of next free slot in the middle should be passed to this function
    int free_slot = -1;
    for (size_t i = 0; i < middle_.size(); ++i) {
        if (middle_[i].first.rank == RANK_NONE && middle_[i].first.suit == SUIT_NONE) {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) {
        std::cerr << "Error: No free slot available in Middle." << std::endl;
        return false;
    }

    // Place the card in the identified slot
    middle_[free_slot].first = card;
    //Update middle number of cards in middle & in player hand
    ++number_cards_middle_;
    // --player_number_of_cards_[PlayerID];

    return 0;
}

//PRE: A valid move 
//POST: Middle, Playerhand & numberofcards in hand updated
void CardManager::defendCard(Card card, unsigned int PlayerID, unsigned int slot){
    //Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card) != player_hands_[PlayerID].end() && "Card not found in the players hand");
    //save position of the card in the players hand
    auto cardPosition = std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card);
    //Place Card
    middle_[slot].second=card;
    //Remove card from the players hand
    player_hands_[PlayerID].erase(cardPosition);
    
    //Update number of cards in middle & in player hand
    ++number_cards_middle_;
    --player_number_of_cards_[PlayerID];
}

//PRE:
//POST: All cards in the middle are moved from "middle" to "discarded cards"
bool CardManager::clearMiddle(){
    assert(middle_.size()<=6 && "middle_ shouldn't have more than six slots");
    //Alli charte transfere vo mitti zu discarded, bin nonig so zfriede mit dere implementation
    while(!middle_.empty()){
        discarded_cards_.push_back(middle_[0].first);
        discarded_cards_.push_back(middle_[0].second);
        middle_.erase(middle_.begin());
    }

    //Azahl charte i de mitti apasse
    number_cards_middle_ = middle_.size();
    return 0;
}   

//PRE:
//POST: All cards in the middle are assigned to the defenders hands
void CardManager::pickUp(unsigned int PlayerID_def){
    //Danil söll ID vom defender mitgeh
    assert(middle_.size()<=6 && "middle shouldn't have more than six slots");
    //Alli charte transfere vo mitti zu discarded, bin nonig so zfriede mit dere implementation
    while(!middle_.empty()){
        player_hands_[PlayerID_def].push_back(middle_[0].first);
        discarded_cards_.push_back(middle_[0].second);
        middle_.erase(middle_.begin());
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
    if (card1.suit == trump_){
        return card2.suit==trump_ && card2.rank > card1.rank;
    }
    return card2.suit==trump_ || (card2.suit==card1.suit && card2.rank > card1.rank);
}

Suit CardManager::getTrump(){
    return trump_;
}


void CardManager::fillDeck() {
    for (int suit = SUIT_CLUBS; suit <= SUIT_HEARTS; ++suit) {  // Iterate over all suits
        for (int rank = RANK_TWO; rank <= RANK_ACE; ++rank) {    // Iterate over all ranks
            deck_.push_back(Card(static_cast<Rank>(rank), static_cast<Suit>(suit)));
        }
    }
}

void CardManager::placeAttackCard(Card card, int slot){
    middle_[slot % 6].first = card;
}
void CardManager::addCardToPlayerHand(unsigned int PlayerID, const Card& card) {
    bool flag = false;
    if (PlayerID < player_hands_.size()) {
        for(int i = 0; i < player_hands_[PlayerID].size(); ++i){
            if (player_hands_[PlayerID][i] == card){
                flag = true; //card is already in the hand
                break;
            }
        }
    }
    if(flag == false){
        player_hands_[PlayerID].push_back(card);
    }
}