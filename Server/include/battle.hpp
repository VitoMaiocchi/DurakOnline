#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <vector>
#include <tuple>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <optional>
#include <iostream>


#include "card_manager.hpp"

#include "../../Networking/include/Networking/util.hpp"
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>

enum BattlePhase {
    BATTLEPHASE_FIRST_ATTACK, //waiting for first attack (no cards in middle)
    BATTLEPHASE_OPEN,         //undefended waiting for pick up, more attacks, or defend
    BATTLEPHASE_DEFENDED,     //defended waiting for ready or more attacks
    BATTLEPHASE_POST_PICKUP,  //waiting for ready or post pickup throw ins
    BATTLEPHASE_DONE          //battle is ready to be destroyed by game (no cards in middle)
};

enum BattleType { //passed on contruction to battle by game
    BATTLETYPE_FIRST, //first battle of the game
    BATTLETYPE_NORMAL, //normal battle
    BATTLETYPE_ENDGAME //endgame battle (only two players left)
};

class Battle {
    private:
        //we might not need that because the other vector saves it already
        // std::vector<int> players; //saves the player ids of the players
        std::map<ClientID, PlayerRole> players_bs_; //attacking, defending, spectating
        std::set<ClientID> finished_players_; //they get the battle state update idle 

        BattlePhase phase;
        BattleType btype_;

        bool defending_flag_ = false;
        int max_attacks_ = 6;
        int curr_attacks_ = 0;
        int attacks_to_defend_ = 0;
        bool first_battle_ = false;
        bool defense_started_ = false; //important flag for passOn function and validating pass on
        //pointer to the player that first layed down a card
        const std::pair<const ClientID, PlayerRole>* first_attacker_ = nullptr;

        //pointer to cardmanager
        CardManager *card_manager_ptr_;

        //Tracks order of attackers in case an attack is passed on, used for pickup function at the end of every battle
        //front is first, back is last to pick up
        std::deque<ClientID> attack_order_;

        //need two ok messages, one from attacker and one from coattacker
        std::map<PlayerRole, bool> ok_msg_ = {{ATTACKER, false}, {CO_ATTACKER, false}};
        //pick up msg
        //its weather or not the message was received, not if its positive or negative
        bool pickUp_msg_ = false;
        bool pickUp_ = false; // if it was actually picked up

        // std::optional 
        std::optional<Card> slot_ = std::nullopt; //now we can check if slot.hasValue() means there is a card

        bool battle_done_;

        //this will save all the cards from the middle, so the attackers can throw in more cards
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> picked_up_cards_;

        void attackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);
        void coAttackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);
        void defenderCardEvent(std::unordered_set<Card> &cards, ClientID clientID, CardSlot slot);

        void doneEvent(ClientID clientID);
        void reflectEvent(ClientID clientID);
        void pickupEvent(ClientID clientID);

        std::optional<Card> getReflectCard(ClientID clientID);
        bool topSlotsClear();
        bool passOnRankMatch(Rank rank);
        void updateAvailableAction();
        void tryPickUp();

    public:
        
        Battle(); //default Constructor
        Battle(BattleType btype, bool first_battle, std::map<ClientID, PlayerRole> players, CardManager &card_manager, std::set<ClientID> finished_players);
        ~Battle(); //default Destructor
        
        //add friend class
        friend class DurakBattleTest;
        friend class DurakGameTest;

        bool handleCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);
        bool handleActionEvent(ClientID player_id, ClientAction action);
        bool successfulDefend();
        bool passOn(Card card, ClientID player_id, CardSlot slot);
        bool isValidMove( const Card &card, ClientID player_id, CardSlot slot);

        // helper functions
        void attack(ClientID client, Card card); //calls the cardmanagers attack function
        void defend(ClientID client, Card card, CardSlot slot); //calls the cardmanagers defend function

        //moves the player roles one to the right and circles around again
        void movePlayerRoles();
        void UpdatePickUpOrder();
        ClientID nextInOrder(ClientID current);
        ClientID findRole(PlayerRole role);

        bool battleIsDone();

        std::map<ClientID, PlayerRole> getPlayerRolesMap();


//setter and getter functions
        void setCurrAttacks(int attacks) { curr_attacks_ = attacks; }
        int getCurrAttacks() const { return curr_attacks_; }

        void setMaxAttacks(int max) { max_attacks_ = max; }
        int getMaxAttacks() const { return max_attacks_; }

        //getter function for testing purposes
        PlayerRole getPlayerRole(ClientID client) { return players_bs_[client];}

        //returns the id of the defender
        ClientID getCurrentDefender();

        void setAttacksToDefend(int atd){attacks_to_defend_ = atd;}

        //returns a pointer to the person who laid down the card the first
        const std::pair<const ClientID, PlayerRole>* getFirstAttackerPtr();

};

#endif
//first_battle

