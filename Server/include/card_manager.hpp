#ifndef CARD_MANAGER_HPP
#define CARD_MANAGER_HPP

#include <vector>
#include <deque>
#include <memory>
#include "Networking/util.hpp"

class cardManager
{
private:
    std::deque<Card> deck;              //represents cards in the middle
    std::shared_ptr<Card> last_card=std::make_shared<Card>();                    //Pointer to the last card in the deck, maybe make it const
    unsigned int number_cards_in_deck;  //Number of cards in the deck
    bool endgame;                       //Could be useful for endgame functions, delete if it is not used
    std::vector<Card> discarded_cards;  //Charte wo "weg" sind
    unsigned int number_discarded_cards; // azahl charte wo "weg" sind
    Suit trump;                           //included from card.hpp if we remove/change this inclusion we have to find another solution
    std::vector<std::vector<Card>> player_hands;
    std::vector<unsigned int> player_number_of_cards;
    std::vector<std::pair<Card,Card>> Middle; //represents the battlefield in the middle
    


public:
    //constructor and destructor
    cardManager(/* args */);
    ~cardManager();

    //At the beginning of the game
    bool shuffleCards();
    bool determineTrump();

    //getter functions
    std::vector<Card> getPlayerHand (unsigned int PlayerID);
    std::vector<std::pair<Card,Card>> getMiddle();
    unsigned int getNumberActivePlayers();
    unsigned int getNumberOfCardsInHand(unsigned int PlayerID);

    // game actions
    bool playCard(Card card, unsigned int PlayerID);   //Playing a card to attack or defend, I might have to add some arguments like the position where the card will be played or if the player is an attacker or a defender 
    bool clearMiddle();         //When attack is succesfully defended
    bool pickUp();              //When attack wasn't succesfully defended
    bool distributeNewCards();
    bool compareCards(Card card1, Card card2);


    
};


#endif