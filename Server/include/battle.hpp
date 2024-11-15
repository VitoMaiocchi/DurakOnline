#ifndef BATTLE_HPP
#define BATTLE_HPP

#include "../../Networking/include/Networking/util.hpp"
#include "card_manager.hpp"
// #include "game.hpp"
#include "msg_handler.hpp"

#include <vector>
#include <tuple>

#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
// #include <Networking/util.hpp>

class Battle {
    private:
        //we might not need that because the other vector saves it already
        // std::vector<int> players; //saves the player ids of the players
        std::unordered_map<ClientID, PlayerRole> players_bs_; //attacking, defending, spectating
        bool defending_flag_ = false;
        int max_attacks_ = 6;
        int curr_attacks_ = 0;
        int attacks_to_defend_ = 0;
        bool first_battle_ = false;
        
        //pointer to the player that first layed down a card
        const std::pair<const ClientID, PlayerRole>* first_attacker_ = nullptr;

        //pointer to cardmanager
        CardManager *card_manager_ptr_;

        //pointer to current game
        // Game *current_game;

    public:
        friend class DurakBattleTest;
        
        Battle(); //default Constructor
        Battle(bool first_battle, std::unordered_map<ClientID, PlayerRole> players, CardManager &card_manager /*,Game &game*/);
        ~Battle(); //default Destructor
        

        bool handleCardEvent(std::vector<Card> cards, int player_id, CardSlot slot);
        bool handleActionEvent();
        bool successfulDefend();
        bool passOn(Card card, int player_id, CardSlot slot);
        bool isValidMove( const Card &card, int player_id, CardSlot slot);

        // helper functions
        void attack(); 
        void defend();

//setter and getter functions
        void setCurrAttacks(int attacks) { curr_attacks_ = attacks; }
        int getCurrAttacks() const { return curr_attacks_; }

        void setMaxAttacks(int max) { max_attacks_ = max; }
        int getMaxAttacks() const { return max_attacks_; }

        //returns a pointer to the person who laid down the card the first
        const std::pair<const ClientID, PlayerRole>* getFirstAttackerPtr();

};

#endif
//first_battle


// (\(\ 
// ( -.-)
// o_(")(")