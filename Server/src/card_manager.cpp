#include <algorithm>
#include <cassert>
#include <random>
#include <iostream>
#include "../include/server.hpp"
#include "../include/card_manager.hpp"


CardManager::CardManager(std::set<ClientID> players) : players(players){
    // initialize the deck with all 52 cards
    fillDeck();

    // initialize the map playerHands with empty vectors to the corresponding players 
    // so we dont access a empty map
    for(ClientID id : players){
        player_hands_[id] = std::vector<Card>();
    }
    
    // shuffle the deck and distribute 6 cards to each player
    shuffleCards();

    // determine the trump suit
    determineTrump();
    // send out a message to all clients with the current status of the cards
    cardUpdate();
}

CardManager::~CardManager() = default;

void CardManager::shuffleCards(){
    // Check if deck has been initialized properly
    assert(deck_.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define pseudo random number generator
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(deck_.begin(), deck_.end(), g);

    
    // distribute cards to player
    for (auto pl : players){
        // Check if players hands are empty
        assert(player_hands_[pl].empty() && "Player's hand should be empty before dealing");

        // distribute the top 6 cards to the player
        player_hands_[pl].insert(player_hands_[pl].begin(), deck_.begin(), deck_.begin()+6);

        // initialize the player number of cards so each player has 6
        player_number_of_cards_[pl] = 6;

        // remove cards from deck
        deck_.erase(deck_.begin(), deck_.begin()+6);
    }

}

void CardManager::determineTrump(){
    // Check if deck has been initialized properly
    assert(deck_.size() == 52 - (6 * player_hands_.size()) && "Deck must contain exactly 52 cards before determining trump");

    // Assign pointer
    last_card_ = std::make_shared<Card>(deck_.back());

    // Check if pointer has been assigned
    assert(last_card_ && "Pointer to last card is void");

    trump_card_ = *last_card_;
    trump_ = trump_card_.suit;
}

std::vector<Card> CardManager::getPlayerHand (ClientID PlayerID){
    return player_hands_[PlayerID];
}

std::vector<std::pair<std::optional<Card>,std::optional<Card>> > CardManager::getMiddle(){
    return middle_;
}

unsigned int CardManager::getNumberActivePlayers(){
    unsigned int activePlayers = 0;
    if(getNumberOfCardsOnDeck()){
        activePlayers = player_hands_.size();
    } else {
        activePlayers = std::count_if(player_hands_.begin(), player_hands_.end(), [](const std::pair<const ClientID, std::vector<Card>>& pair) {
            return !pair.second.empty();
        });
    }

    return activePlayers;
}

unsigned int CardManager::getNumberOfCardsInHand(ClientID PlayerID){
    return player_hands_[PlayerID].size();
}

unsigned int CardManager::getNumberOfCardsOnDeck() const{
    return deck_.size();
}

void CardManager::attackCard(Card card, ClientID PlayerID){
    // Check that this card is actually in the players Hands
    assert(std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card) != player_hands_[PlayerID].end() && "Card not found in the players hand");
    // save position of the card in the players hand
    auto cardPosition = std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card);
    
    // Position of next free slot in the middle should be passed to this function
    int free_slot = -1;
    for (size_t i = 0; i < 6; ++i) {
        if (!middle_[i].first.has_value()) {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) {
        std::cerr << "Error: No free slot available in Middle." << std::endl;
    }

    // Place the card in the identified slot
    middle_[free_slot].first = card;

    // remove card from player hand
    player_hands_[PlayerID].erase(cardPosition);

    // Update middle number of cards in middle & in player hand
    ++number_cards_middle_;
    --player_number_of_cards_[PlayerID];
}

void CardManager::defendCard(Card card, ClientID PlayerID, unsigned int slot){
    // Check that this card is actually in the players Hands (We might want to move this to the isValidMove function in battle)
    assert (std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card) != player_hands_[PlayerID].end() && "Card not found in the players hand");
    // save position of the card in the players hand
    auto cardPosition = std::find(player_hands_[PlayerID].begin(), player_hands_[PlayerID].end(), card);
    // Place Card
    middle_[slot % 6].second=card;
    
    // Remove card from the players hand
    player_hands_[PlayerID].erase(cardPosition);
    
    // Update number of cards in middle & in player hand
    ++number_cards_middle_;
    --player_number_of_cards_[PlayerID];

}

void CardManager::clearMiddle(){
    assert(middle_.size() <=6 && "middle_ shouldn't have more than six slots");
    assert(middle_.size() > 0 && "Middle is empty, this function shouldn't be called on an empty middle");
    // if the middle is not empty 
    if(!middle_.empty()){
        // iterate over the middle
        for(auto& slot : middle_){
            // discard the bottom card
            if(slot.first.has_value()){
                discarded_cards_.push_back(slot.first.value());
            }
            // discard the top card
            if(slot.second.has_value()){
                discarded_cards_.push_back(slot.second.value());
            }
            // clear the pair 
            slot = {std::nullopt, std::nullopt};
        }
    }
    
    // update number of cards in middle
    number_cards_middle_ = middle_.size();
}   

void CardManager::pickUp(ClientID playerID_def){

    assert(middle_.size()<=6 && "middle shouldn't have more than six slots");

    // if the middle is not empty
    if(!middle_.empty()){
        // iterate over the middle
        for(auto& slot : middle_){
            // place the bottom card into the defenders hand
            if(slot.first.has_value()){
                player_hands_[playerID_def].push_back(slot.first.value());
                player_number_of_cards_[playerID_def]++;
            }
            // place the top card into the defenders hand
            if(slot.second.has_value()){
                player_hands_[playerID_def].push_back(slot.second.value());
                player_number_of_cards_[playerID_def]++;
            }
            // set slot to no value top and bottom 
            slot = {std::nullopt, std::nullopt};
        }
    }
}   

void CardManager::distributeNewCards(std::deque<ClientID> attack_order_, ClientID current_defender, bool succesful_defend){
    // attackers pick up cards
    while (getNumberOfCardsOnDeck() && !attack_order_.empty()){
        drawFromMiddle(attack_order_.front());
        attack_order_.pop_front();
    }
    // defender only picks up if all attacks where defended succesfully
    if (succesful_defend){
        drawFromMiddle(current_defender);
    }
}

void CardManager::drawFromMiddle(ClientID playerID){
    // If there are cards in the deck and the player has less than six cards, assign cards to the players hand until the player has 6
    while (getNumberOfCardsInHand(playerID) < 6 && getNumberOfCardsOnDeck()){
        player_hands_[playerID].push_back(deck_.front());
        deck_.pop_front();
        --number_cards_in_deck_;
        ++player_number_of_cards_[playerID];
    }
}

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

void CardManager::addCardToPlayerHand(ClientID playerID, const Card& card) {
    bool flag = false;

    for(int i = 0; i < player_hands_[playerID].size(); ++i){
        if (player_hands_[playerID].at(i)== card){
            flag = true; //card is already in the hand
            break;
        }
    }
    if(flag == false){
        player_hands_[playerID].push_back(card);
        player_number_of_cards_[playerID]++;
    }
}

void CardManager::cardUpdate() {
    CardUpdate card_message;
    for(ClientID pl : players) card_message.opponent_cards[pl] = player_hands_[pl].size();
    card_message.draw_pile_cards = deck_.size();
    card_message.trump_card = trump_card_;
    card_message.trump_suit = trump_;

    // iterate trough middle cards
    for(int i = 0; i < middle_.size(); ++i){
        if(middle_[i].first.has_value()){
            card_message.middle_cards[static_cast<CardSlot>(i)] = middle_[i].first.value();
        }
        if(middle_[i].second.has_value()){
            card_message.middle_cards[static_cast<CardSlot>(i + 6)] = middle_[i].second.value();
        }
    }

    // send a update to all players
    for(ClientID client : DurakServer::clients) {
        card_message.hand = std::list<Card>(player_hands_[client].begin(), player_hands_[client].end());
        Network::sendMessage(std::make_unique<CardUpdate>(card_message), client);
    }
}

// bool CardManager::gameIsOver(){
//     return getNumberActivePlayers() == 1;
// }

// bool CardManager::playerFinished(ClientID playerID){
//     return (!getNumberOfCardsInHand(playerID) && !getNumberOfCardsOnDeck());
// }

std::optional<Card> CardManager::getMiddleSlot(uint slot) {
    auto stack = middle_[slot%6];
    if(slot/6) return stack.second; //top
    else return stack.first;        //bottom
}

void CardManager::eraseDeck() {
    deck_.clear();
}
