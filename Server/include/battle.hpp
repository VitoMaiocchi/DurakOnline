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
#include <Networking/util.hpp>

class Battle {
    private:
        //we might not need that because the other vector saves it already
        // std::vector<int> players; //saves the player ids of the players
        std::vector<std::pair<int, PlayerRole>> players_bs; //attacking, defending, spectating
        bool defending_flag;
        int max_attacks;
        int curr_attacks;
        int attacks_to_defend = 0;
        bool first_battle_ = false;;
        
        //pointer to the player that first layed down a card
        std::pair<int, PlayerRole>* first_attacker = nullptr;

        //pointer to cardmanager
        CardManager *card_manager_ptr;

        //pointer to current game
        // Game *current_game;

    public:
        Battle(); //default Constructor
        Battle(bool first_battle, std::vector<std::pair<int, PlayerRole>> players, CardManager &card_manager /*,Game &game*/);
        ~Battle(); //default Destructor
        

        bool handleCardEvent(std::vector<Card> cards, int player_id, CardSlot slot);
        bool handleActionEvent();
        bool successfulDefend();
        bool passOn(Card card, int player_id, CardSlot slot);
        bool isValidMove( const Card &card, int player_id, CardSlot slot);

        // helper functions
        void attack(); 
        void defend();
};

#endif
//first_battle


// (\(\ 
// ( -.-)
// o_(")(")