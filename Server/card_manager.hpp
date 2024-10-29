#ifndef CARD_MANAGER_HPP
#define CARD_MANAGER_HPP

#include "../Networking/include/Networking/util.hpp"
#include <vector>
#include <deque>

class card_manager
{
private:
    std::deque<Card> deck;              //represents cards in the middle
    *Card last_card;                    //Pointer to the last card in the deck, maybe make it const
    unsigned int number_cards_in_deck;  //Number of cards in the deck
    bool endgame;                       //Could be useful for endgame functions, delete if it is not used
    std::vector<Card> discarded_cards;  //Charte wo "weg" sind
    unsigned int number_discarded_cards; // azahl charte wo "weg" sind
    enum trump{Hearts, Clubs, Diamonds, Spades};                 //trump suit
    std::vector<std::vector<Card>> player_hands;
    std::vector<unsigned int> player_number_of_cards;
    std::vector<std::pair<Card,Card>> Middle; //represents the battlefield in the middle



public:
    //constructor and destructor
    card_manager(/* args */);
    ~card_manager();

    //At the beginning of the game
    bool shuffleCards();
    bool determineTrump();

    //getter functions
    std::vector<unsigned int> getPlayerHand (unsigned int PlayerID);
    std::vector<std::pair<Card,Card>> getMiddle();
    unsigned int getNumberActivePlayers();
    unsigned int getNumberOfCardsInHand(unsigned int PlayerID)

    // game actions
    bool playCard(Card card, unsigned int PlayerID);   //Playing a card to attack or defend, I might have to add some arguments like the position where the card will be played or if the player is an attacker or a defender 
    bool clearMiddle();         //When attack is succesfully defended
    bool pickUp();              //When attack wasn't succesfully defended
    bool distributeNewCards();
    bool compareCards(Card card1, Card card2);


    
};


#endif