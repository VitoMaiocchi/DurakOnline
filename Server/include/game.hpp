#pragma once

#include "card_manager.hpp"
#include "../../Networking/include/Networking/util.hpp"
#include "../../Networking/include/Networking/message.hpp"
#include "battle.hpp"

#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <optional>

using player_id = unsigned int;

class Game{

    private:
        /**
         * @brief vector of pairs containing the player ids and their roles 
         * has to be stored here because battle will be destructed and created again
         * 
         */
        std::map<ClientID, PlayerRole> player_roles_;

        /**
         * @brief current battle which is created when an attack is started and destroyed when the battle is over
         * 
         */
        std::optional<Battle> current_battle_ = std::nullopt;
        
        /**
         * @brief card manager for the current game, handles all card related functions (shuffling, dealing, etc.)
         * 
         */
        CardManager card_manager_;


        /**
         * @brief set of player ids that have finished the game
         * 
         */
        std::set<ClientID> finished_players_;
        
        bool isgameover_ = false;

    public:

        /**
         * @brief Construct a new Game object
         * 
         * @param players set of player ids that are in the game
         */
        Game(std::set<ClientID> &players);

        friend class DurakGameTest;

        /**
         * @brief creates a new battle object depending on how many players are left in the game (normal or endgame where endgame is only two players left)
         * 
         */
        void createBattle();

        // bool isStarted();
        // bool resetGame();
        // void updateTurnOrder();
        // void updateFinishedPlayers();
        
        /**
         * @brief handles the client action event (pick up, pass on, ok)
         * 
         * @param message message containing the action
         * @param client client id of the player that sent the message
         * 
         */
        void handleClientActionEvent(std::unique_ptr<Message> message, ClientID client);

        /**
         * @brief handles the client card event (attack, defend)
         * 
         * @param message message containing the card event
         * @param client client id of the player that sent the message
         * @return ClientID client id of the durak if the game is over, 0 otherwise
         */
        ClientID handleClientCardEvent(std::unique_ptr<Message> message, ClientID client);

        /**
         * @brief finds the last player that has cards left in his hand
         * 
         * @return ClientID client id of the last player aka the durak
         */
        ClientID findlastplayer();

        //
        // setter and getter functions
        //
        
            // getter functions
        /**
         * @brief checks if the game is over
         * 
         * @return true if the game is over, false otherwise
         */
        bool isgameOver();

        /**
         * @brief getter function for the current battle
         * 
         * @return Battle* pointer to the current battle
         */
        Battle* getCurrentBattle(){ return &current_battle_.value();}

        /**
         * @brief getter function for the card manager
         * 
         * @return CardManager* pointer to the card manager
         */
        CardManager* getCardManager(){ return &card_manager_;}

        /**
         * @brief getter function for the player roles
         * 
         * @return std::map<ClientID, PlayerRole> map containing the player ids and their roles
         */
        std::map<ClientID, PlayerRole> getPlayerRoles(){ return player_roles_;}

};
