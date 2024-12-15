#pragma once

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
#include "../../Networking/include/Networking/message.hpp"
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>

/**
 * @brief The BattlePhase enum is used to represent the different phases of a battle in the game. 
 * Each phase indicates a specific state of the battle, which helps in managing the flow of the game 
 * and determining the valid actions that players can take.
 */
enum BattlePhase {
    BATTLEPHASE_FIRST_ATTACK, // waiting for first attack (no cards in middle)
    BATTLEPHASE_OPEN,         // undefended, waiting for pick up, more attacks, or defend
    BATTLEPHASE_DEFENDED,     // defended waiting for ready or more attacks
    BATTLEPHASE_POST_PICKUP,  // waiting for ready or post pickup throw ins
    BATTLEPHASE_DONE          // battle is ready to be destroyed by game (no cards in middle)
};

/**
 * @brief The BattleType enum is used to specify the type of battle being played. 
 * This helps in determining specific rules and behaviors for different stages of the game.
 */
enum BattleType { 
    BATTLETYPE_FIRST,  // First battle of the game
    BATTLETYPE_NORMAL, // Normal battle
    BATTLETYPE_ENDGAME // Endgame battle (only two players left)
};

class Battle {

    private:

        ///////////////////////////
        // private member variables
        ///////////////////////////

        std::map<ClientID, PlayerRole> players_bs_; // Player roles of clients
        std::set<ClientID> finished_players_; // set of finished players

        BattlePhase phase_;
        BattlePhase previous_phase_;
        BattleType btype_;

        bool move_could_end_game_ = false;
        bool gameover_ = false;
        bool defending_flag_ = false;
        int max_attacks_ = 6;
        int curr_attacks_ = 0;
        int attacks_to_defend_ = 0;
        bool first_battle_ = false;
        bool defense_started_ = false; // important flag for passOn function and validating pass on

        // pointer to the player that first layed down a card
        const std::pair<const ClientID, PlayerRole>* first_attacker_ = nullptr;

        // pointer to cardmanager
        CardManager *card_manager_ptr_;

        // Tracks order of attackers in case an attack is passed on, used for pickup function at the end of every battle
        // front is first, back is last to pick up
        std::deque<ClientID> attack_order_;

        // need two ok messages, one from attacker and one from coattacker
        std::map<PlayerRole, bool> ok_msg_ = {{ATTACKER, false}, {CO_ATTACKER, false}};

        // pick up msg
        // its wether or not the message was received, not if its positive or negative
        bool pickUp_msg_ = false;
        bool pickUp_ = false; // if it was actually picked up

        // std::optional 
        std::optional<Card> slot_ = std::nullopt; //now we can check if slot.hasValue() means there is a card

        bool battle_done_;

        // this will save all the cards from the middle, so the attackers can throw in more cards
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> picked_up_cards_;

        ///////////////////////////
        // private member functions
        ///////////////////////////

        /**
         * @brief Handles the event when an attacker plays cards.
         * 
         * This function processes the cards played by an attacker, validates the move,
         * and updates the game state accordingly. It ensures that the move is legal
         * and updates the battle phase.
         * 
         * @param cards The cards being played.
         * @param player_id The ID of the player playing the cards.
         * @param slot The slot where the card is being placed.
         */
        void attackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);

        /**
         * @brief Handles the event when a co-attacker plays cards.
         * 
         * This function processes the cards played by a co-attacker, validates the move,
         * and updates the game state accordingly. It ensures that the move is legal
         * and updates the battle phase.
         * 
         * @param cards The cards being played.
         * @param player_id The ID of the player playing the cards.
         * @param slot The slot where the card is being placed.
         */
        void coAttackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);

        /**
         * @brief Handles the event when a defender plays cards.
         * 
         * This function processes the cards played by a defender, validates the move,
         * and updates the game state accordingly. It ensures that the move is legal
         * and updates the battle phase.
         * 
         * @param cards The cards being played.
         * @param clientID The ID of the player playing the cards.
         * @param slot The slot where the card is being placed.
         */
        void defenderCardEvent(std::unordered_set<Card> &cards, ClientID clientID, CardSlot slot);

        /**
         * @brief Handles the event when a player indicates they are done with their turn.
         * 
         * This function processes the done action from a player, updates the game state,
         * and checks if the battle phase needs to be advanced or if any other actions
         * need to be taken.
         * 
         * @param clientID The ID of the player who is done.
         */
        void doneEvent(ClientID clientID);

        /**
         * @brief Handles the event when a defender reflects the attack to the next player.
         * 
         * This function processes the reflect action from a defender, updates the game state,
         * and changes the player roles accordingly.
         * 
         * @param clientID The ID of the player reflecting the attack.
         */
        void reflectEvent(ClientID clientID);

        /**
         * @brief Handles the event when a defender picks up the cards.
         * 
         * This function processes the pick-up action from a defender, updates the game state,
         * and changes the player roles accordingly.
         * 
         * @param clientID The ID of the player picking up the cards.
         */
        void pickupEvent(ClientID clientID);

        /**
         * @brief Get the Reflect Card object if the defender can reflect the attack.
         * 
         * This function checks if the defender can reflect the attack based on the current
         * game state and returns the card to reflect with, if available.
         * 
         * @param clientID The ID of the player attempting to reflect.
         * @return std::optional<Card> The card to reflect with, if available.
         */
        std::optional<Card> getReflectCard(ClientID clientID);

        /**
         * @brief Check if the top slots (defense slots) are clear.
         * 
         * This function checks if all the top slots (defense slots) are clear, which is
         * necessary for certain actions like reflecting the attack.
         * 
         * @return true if all top slots are clear, false otherwise.
         */
        bool topSlotsClear();

        /**
         * @brief Check if all cards in the middle match the given rank.
         * 
         * This function checks if all the cards in the middle of the battlefield match
         * the given rank, which is necessary for certain actions like passing on the attack.
         * 
         * @param rank The rank to check against.
         * @return true if all cards match the rank, false otherwise.
         */
        bool passOnRankMatch(Rank rank);

        /**
         * @brief Update the available actions for all players based on the current game state.
         * 
         * This function updates the available actions for all players based on the current
         * game state and sends the updated actions to the clients.
         */
        void updateAvailableAction();

        /**
         * @brief Attempt to pick up the cards in the middle.
         * 
         * This function attempts to pick up the cards in the middle of the battlefield,
         * updates the game state, and changes the player roles accordingly.
         */
        void tryPickUp();

        /**
         * @brief Remove players who have finished the game from the active player list.
         * 
         * This function removes players who have finished the game from the active player list
         * and updates the game state accordingly.
         */
        void removeFinishedPlayers();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    public:
        
        ///////////////////////////////
        // Constructors and Destructors
        ///////////////////////////////

        Battle(); // default Constructor
        Battle(BattleType btype, std::map<ClientID, PlayerRole> players, CardManager &card_manager, std::set<ClientID> finished_players);
        ~Battle(); // default Destructor
        
        // add friend class
        friend class DurakBattleTest;
        friend class DurakGameTest;

        //////////////////////////
        // public member functions
        //////////////////////////

        /**
         * @brief Handles the event when a player plays cards.
         * 
         * Processes the cards played by a player, validates the move, and updates the game state accordingly.
         * 
         * @param cards The cards being played.
         * @param player_id The ID of the player playing the cards.
         * @param slot The slot where the card is being placed.
         * @return true if the move is valid, false otherwise.
         */
        bool handleCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot);

        /**
         * @brief Handles the event when a player performs an action.
         * 
         * Processes the action performed by a player, updates the game state, and sends necessary updates to clients.
         * 
         * @param player_id The ID of the player performing the action.
         * @param action The action being performed.
         */
        void handleActionEvent(ClientID player_id, ClientAction action);

        /**
         * @brief Checks if the defender has successfully defended all attacks.
         * 
         * @return true if all attacks have been defended, false otherwise.
         */
        bool successfulDefend();

        /**
         * @brief Handles the event when a defender passes on the attack to the next player.
         * 
         * Validates the move and updates the game state accordingly.
         * 
         * @param cards The cards being played.
         * @param player_id The ID of the player passing on the attack.
         * @param slot The slot where the card is being placed.
         * @return true if the move is valid, false otherwise.
         */
        bool passOn(std::unordered_set<Card> &cards, ClientID player_id, CardSlot slot);

        /**
         * @brief Checks if a move is valid.
         * 
         * Validates the card played by a player and ensures it follows the game rules.
         * 
         * @param card The card being played.
         * @param player_id The ID of the player playing the card.
         * @param slot The slot where the card is being placed.
         * @return true if the move is valid, false otherwise.
         */
        bool isValidMove(const Card &card, ClientID player_id, CardSlot slot);

        /**
         * @brief Handles the event when an attacker plays a card.
         * 
         * Calls the card manager's attack function and updates the game state.
         * 
         * @param client The ID of the player attacking.
         * @param card The card being played.
         */
        void attack(ClientID client, Card card);

        /**
         * @brief Handles the event when a defender plays a card.
         * 
         * Calls the card manager's defend function and updates the game state.
         * 
         * @param client The ID of the player defending.
         * @param card The card being played.
         * @param slot The slot where the card is being placed.
         */
        void defend(ClientID client, Card card, CardSlot slot);

        /**
         * @brief Checks if the maximum number of attacks have been played.
         * 
         * Returns true if there are at least 6 cards attacking the current defender in a normal battle, 5 in the first battle.
         * 
         * @return true if the maximum number of attacks have been played, false otherwise.
         */
        bool attackedWithMaxCards();

        /**
         * @brief Moves the player roles one position to the right and circles around.
         * 
         * Updates the roles of players and sends necessary updates to clients.
         */
        void movePlayerRoles();

        /**
         * @brief Updates the order of players who need to pick up cards.
         * 
         * Adjusts the attack order deque based on the current game state.
         */
        void UpdatePickUpOrder();

        /**
         * @brief Returns the ID of the next player in order.
         * 
         * Finds the next active player in the player roles map.
         * 
         * @param current The ID of the current player.
         * @return ClientID The ID of the next player.
         */
        ClientID nextInOrder(ClientID current);

        /**
         * @brief Returns an iterator to the next active player in order.
         * 
         * Finds the next active player in the player roles map.
         * 
         * @param it The iterator pointing to the current player.
         * @return std::map<ClientID, PlayerRole>::iterator The iterator pointing to the next player.
         */
        std::map<ClientID, PlayerRole>::iterator nextInOrderIt(std::map<ClientID, PlayerRole>::iterator it);

        /**
         * @brief Finds the player with the specified role.
         * 
         * Returns the ClientID of the player with the given role.
         * 
         * @param role The role to find.
         * @return ClientID The ID of the player with the given role.
         */
        ClientID findRole(PlayerRole role);

        /**
         * @brief Checks if the battle is done.
         * 
         * Returns true if the battle is finished, false otherwise.
         * 
         * @return true if the battle is done, false otherwise.
         */
        bool battleIsDone();

        /**
         * @brief Returns the map of player roles.
         * 
         * Provides the current roles of all players in the battle.
         * 
         * @return std::map<ClientID, PlayerRole> The map of player roles.
         */
        std::map<ClientID, PlayerRole> getPlayerRolesMap();

        //////////////////////////////
        // setter and getter functions
        //////////////////////////////

        /**
         * @brief Set the current number of attacks.
         * 
         * @param attacks The number of current attacks.
         */
        void setCurrAttacks(int attacks) { curr_attacks_ = attacks; }

        /**
         * @brief Get the current number of attacks.
         * 
         * @return int The number of current attacks.
         */
        int getCurrAttacks() const { return curr_attacks_; }

        /**
         * @brief Set the maximum number of attacks.
         * 
         * @param max The maximum number of attacks.
         */
        void setMaxAttacks(int max) { max_attacks_ = max; }

        /**
         * @brief Get the maximum number of attacks.
         * 
         * @return int The maximum number of attacks.
         */
        int getMaxAttacks() const { return max_attacks_; }

        /**
         * @brief Check if the game is over.
         * 
         * @return true if the game is over, false otherwise.
         */
        bool isgameover() { return gameover_; }

        //////////////////////////////////////////////////
        // setter and getter function for testing purposes
        //////////////////////////////////////////////////

        /**
         * @brief Get the role of a player.
         * 
         * @param client The ID of the player.
         * @return PlayerRole The role of the player.
         */
        PlayerRole getPlayerRole(ClientID client) { return players_bs_[client];}

        /**
         * @brief Get the ID of the current defender.
         * 
         * @return ClientID The ID of the current defender.
         */
        ClientID getCurrentDefender();

        /**
         * @brief Set the number of attacks to defend.
         * 
         * @param atd The number of attacks to defend.
         */
        void setAttacksToDefend(int atd){attacks_to_defend_ = atd;}

        /**
         * @brief Get a pointer to the first attacker.
         * 
         * @return const std::pair<const ClientID, PlayerRole>* Pointer to the first attacker.
         */
        const std::pair<const ClientID, PlayerRole>* getFirstAttackerPtr();

        /**
         * @brief Sets the player roles for the battle.
         * 
         * @param battlestate A map of ClientID to PlayerRole representing the player roles.
         */
        void setPlayerRoles(std::map<ClientID, PlayerRole> battlestate);

        /**
         * @brief Calls the function to remove finished players from the battle.
         */
        void callRemoveFinishedPlayers();

        ////////////////////
        // message functions
        ////////////////////

        /**
         * @brief Sends an update of the battle state to all clients.
         * 
         * Prepares and sends the BattleStateUpdate message to clients.
         */
        void sendBattleStateUpdate();
};