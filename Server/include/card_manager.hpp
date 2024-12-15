#pragma once

#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <algorithm>
#include <optional>

#include "../../Networking/include/Networking/util.hpp"
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>

class CardManager{

private:

    //
    // private member variables
    //
    bool endgame_;                          // Could be useful for endgame functions, delete if it is not used
    unsigned int number_cards_in_deck_;     // Number of cards in the deck
    unsigned int number_discarded_cards_;   // Number of cards in the discarded pile
    unsigned int number_cards_middle_ = 0;  // Number of cards in the middle
    Suit trump_;                            // Trump suit
    Card trump_card_;

    std::vector<Card> discarded_cards_;                        // cards that are burned after being successfully beaten
    std::shared_ptr<Card> last_card_=std::make_shared<Card>(); // Pointer to the last card in the deck, maybe make it const
    
    std::deque<Card> deck_;              // represents cards in the middle
    std::set<ClientID> players;
    std::map<ClientID, std::vector<Card>> player_hands_;
    std::map<ClientID, unsigned int> player_number_of_cards_; //DEPRECATED NO NOT USE


    // middle has always six slot pairs, top and bottom, it is initialized as std::nullopt meaning it has no value
    std::vector<std::pair<std::optional<Card>,std::optional<Card>>> middle_ = 
            std::vector<std::pair<std::optional<Card>, std::optional<Card>>>(6, {std::nullopt, std::nullopt}); // represents the battlefield in the middle

public:

    /**
     * @brief Construct a new Card Manager object
     * 
     * @param players set of client ids that are in the game
     */
    CardManager(std::set<ClientID> players);

    /**
     * @brief Destroys the Card Manager object
     * 
     */
    ~CardManager();
    
    /**
     * @brief broadcasts the card update to all players
     * 
     */
    void cardUpdate();    

    /**
     * @brief shuffles the deck at the beginning of the game
     * 
     */
    void shuffleCards();

    /**
     * @brief determines the trump suit
     * 
     */
    void determineTrump();

    //
    // getter functions
    //

    /**
     * @brief getter function for the player hand
     * 
     * @param PlayerID client id of the player
     * @return vector of cards in the players hand
     */
    std::vector<Card> getPlayerHand (ClientID PlayerID);

    /**
     * @brief getter function for the middle
     * 
     * @return vector of pairs representing the middle
     */
    std::vector<std::pair<std::optional<Card>,std::optional<Card>>> getMiddle();

    /**
     * @brief getter function for the number of active players
     * 
     * @return number of active players
     */
    unsigned int getNumberActivePlayers();

    /**
     * @brief getter function for the number of cards in a players hand
     * 
     * @param PlayerID client id of the player
     * @return number of cards in the players hand
     */
    unsigned int getNumberOfCardsInHand(ClientID playerID);

    /**
     * @brief getter function for the number of cards in the deck
     * 
     * @return number of cards in the deck
     */
    unsigned int getNumberOfCardsOnDeck() const;

    /**
     * @brief getter function for the trump suit
     * 
     * @return Suit trump suit
     */
    Suit getTrump();

    /**
     * @brief performs player attack with a card
     * 
     * @param card of the player
     * @param playerID of the client performing the attack
     */
    void attackCard(Card card, ClientID playerID);

    /**
     * @brief performs player defense with a card
     * 
     * @param card of the player
     * @param playerID of the client performing the defense
     * @param slot of the card in the middle
     */
    void defendCard(Card card, ClientID playerID, unsigned int slot);

    //
    // game actions
    bool playCard(Card card, ClientID playerID);   //Playing a card to attack or defend, I might have to add some arguments like the position where the card will be played or if the player is an attacker or a defender 
    bool clearMiddle();         //When attack is succesfully defended
    void pickUp(ClientID playerID_def);              //When attack wasn't succesfully defended
    void distributeNewCards(ClientID first, std::map<ClientID, PlayerRole> players);
    void drawFromMiddle(ClientID playerID);

    /**
     * @brief 
     * 
     * @param card1 card to compare
     * @param card2 to this card
     * @return true if card2 is greater than card1, false otherwise
     * 
     */
    bool compareCards(Card card1, Card card2);

    /**
     * @brief fills the deck with cards
     * 
     */
    void fillDeck();

    //
    // functions for testing purposes
    //

    /**
     * @brief places an attack card in the middle
     * 
     * @param card card to place
     * @param slot slot in the middle
     */
    void placeAttackCard(Card card, int slot);

    /**
     * @brief adds a card to a players hand
     * 
     * @param playerID client id of the player
     * @param card card to add
     */
    void addCardToPlayerHand(ClientID playerID, const Card& card);

    /**
     * @brief getter function for the last card in the deck
     * 
     * @return Card last card in the deck
     */
    Card getLastCard(){ return *last_card_;}

    /**
     * @brief get the middle slot
     * 
     * @param slot number of the slot
     * @return card in the middle slot
     */
    std::optional<Card> getMiddleSlot(uint slot);

    /**
     * @brief erases the deck so that we can test the endgame with only two players
     * 
     */
    void eraseDeck();
};