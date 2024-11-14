#ifndef CARD_MANAGER_HPP
#define CARD_MANAGER_HPP

#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include "../../Networking/include/Networking/util.hpp"

class CardManager
{
private:
    std::deque<Card> deck_;              //represents cards in the middle
    std::shared_ptr<Card> last_card_=std::make_shared<Card>();                    //Pointer to the last card in the deck, maybe make it const
    unsigned int number_cards_in_deck_;  //Number of cards in the deck
    bool endgame_;                       //Could be useful for endgame functions, delete if it is not used
    std::vector<Card> discarded_cards_;  //Charte wo "weg" sind
    unsigned int number_discarded_cards_; // azahl charte wo "weg" sind
    Suit trump_;                           //included from card.hpp if we remove/change this inclusion we have to find another solution
    std::vector<std::vector<Card>> player_hands_;
    std::vector<unsigned int> player_number_of_cards_;

    Card empty_card = Card(RANK_NONE, SUIT_NONE);
    std::vector<std::pair<Card,Card>> middle_ = std::vector<std::pair<Card, Card>>(6, {empty_card, empty_card}); //represents the battlefield in the middle
    unsigned int number_cards_middle_ = 0; //Number of cards in the middle
    


public:
    //constructor and destructor
    //Constructor where the number of players is passed
    CardManager(std::vector<ClientID> player_ids);
    ~CardManager();

    //At the beginning of the game
    void shuffleCards();
    void determineTrump();

    //getter functions
    std::vector<Card> getPlayerHand (unsigned int PlayerID);
    std::vector<std::pair<Card,Card>> getMiddle();
    unsigned int getNumberActivePlayers();
    unsigned int getNumberOfCardsInHand(unsigned int PlayerID);
    Suit getTrump();

    bool attackCard(Card card, unsigned int PlayerID);
    void defendCard(Card card, unsigned int PlayerID, unsigned int slot);

    // game actions
    bool playCard(Card card, unsigned int PlayerID);   //Playing a card to attack or defend, I might have to add some arguments like the position where the card will be played or if the player is an attacker or a defender 
    bool clearMiddle();         //When attack is succesfully defended
    void pickUp(unsigned int PlayerID_def);              //When attack wasn't succesfully defended
    bool distributeNewCards();
    bool compareCards(Card card1, Card card2);

    void fillDeck();
    void placeAttackCard(Card card, int slot);
    void addCardToPlayerHand(unsigned int PlayerID, const Card& card);

};


#endif