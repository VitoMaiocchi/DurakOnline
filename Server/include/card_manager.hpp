#ifndef CARD_MANAGER_HPP
#define CARD_MANAGER_HPP

#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <algorithm>
#include <optional>

#include "../../Networking/include/Networking/util.hpp"
//include to send messages
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>

class CardManager
{
private:
    std::deque<Card> deck_;              //represents cards in the middle
    //TODO: replace with optional
    std::shared_ptr<Card> last_card_=std::make_shared<Card>(); //Pointer to the last card in the deck, maybe make it const
    unsigned int number_cards_in_deck_;  //Number of cards in the deck
    bool endgame_;                       //Could be useful for endgame functions, delete if it is not used
    std::vector<Card> discarded_cards_;  //Charte wo "weg" sind
    unsigned int number_discarded_cards_; // azahl charte wo "weg" sind
    Suit trump_;                          //included from card.hpp if we remove/change this inclusion we have to find another solution
    //TODO: bruchemer das?
    Card trump_card_;
    
    std::set<ClientID> players;
    //TODO: evtl replace vector with set es burcht denn halt en < operator und so aber isch effizienter (nöd mega nötig)
    std::map<ClientID, std::vector<Card>> player_hands_;
    std::map<ClientID, unsigned int> player_number_of_cards_; //DEPRECATED NO NOT USE


    //middle has always six slot pairs, top and bottom, it is initialized as std::nullopt meaning it has no value
    //TODO: typedef für middle
    std::vector<std::pair<std::optional<Card>,std::optional<Card>>> middle_ = 
                                        std::vector<std::pair<std::optional<Card>, std::optional<Card>>>(6, {std::nullopt, std::nullopt}); //represents the battlefield in the middle
    
    unsigned int number_cards_middle_ = 0; //Number of cards in the middle


//TODO: Change bool functions to void functions with assert
public:
    //constructor and destructor
    //Constructor where the number of players is passed
    CardManager(std::set<ClientID> players);
    ~CardManager();
    
    void cardUpdate();    

    //At the beginning of the game
    void shuffleCards();
    void determineTrump();

    //getter functions
    std::vector<Card> getPlayerHand (ClientID PlayerID);
    std::vector<std::pair<std::optional<Card>,std::optional<Card>>> getMiddle();
    unsigned int getNumberActivePlayers();
    unsigned int getNumberOfCardsInHand(ClientID playerID);
    unsigned int getNumberOfCardsOnDeck() const;
    bool gameIsOver();
    bool playerFinished(ClientID playerID);
    Suit getTrump();

    bool attackCard(Card card, ClientID playerID);
    void defendCard(Card card, ClientID playerID, unsigned int slot);

    // game actions
    bool playCard(Card card, ClientID playerID);   //Playing a card to attack or defend, I might have to add some arguments like the position where the card will be played or if the player is an attacker or a defender 
    bool clearMiddle();         //When attack is succesfully defended
    void pickUp(ClientID playerID_def);              //When attack wasn't succesfully defended
    void distributeNewCards(std::deque<ClientID> attack_order_, ClientID current_defender, bool succesful_defend);
    void drawFromMiddle(ClientID playerID);
    bool compareCards(Card card1, Card card2);

    void fillDeck();

    // functions for testing purposes
    void placeAttackCard(Card card, int slot);
    void addCardToPlayerHand(ClientID playerID, const Card& card);

    //getter function for testing purposes
    Card getLastCard(){ return *last_card_;}

    std::optional<Card> getMiddleSlot(uint slot);
};


#endif