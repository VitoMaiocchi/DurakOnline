#include "../include/battle.hpp"
#include <iostream>


/**
* QUESTIONS: 
 */

//constructor, passes if it is first battle or not and passes the players with their roles
Battle::Battle(bool first_battle, std::vector<std::pair<int, PlayerRole>> players, CardManager &card_manager /*,Game &game*/) : 
                                    first_battle_(first_battle), players_bs(players), card_manager_ptr(&card_manager) /*,current_game(&game)*/,curr_attacks(0){
    
    max_attacks = first_battle ? 5 : 6;
    //set the first attacker pointer to the one that attacks first
    for(auto& pl : players){
        if(pl.second == ATTACKER){
            first_attacker = &pl;
            break;
        }
    }

    //check that the attacker was found
    if(first_attacker == nullptr){
        std::cerr << "Error: 'attacker' not found" <<std::endl;
    }
};

//default dtor
Battle::~Battle() = default;

bool Battle::handleCardEvent(std::vector<Card> cards, int player_id, CardSlot slot){

    std::cout << "handleCardEvent was called" << std::endl;

    //calls isvalidmove and gives the message parameters with the function,
    //here we should handle the list and maybe break it down? by card, then we can call isValidMove multiple 
    //times and check if each card is valid, and if yes, then we give to go
    
    //if(isValidMove()){
    // check role and call attack() or defend()}
    //do i need to check if the slot is empty? or can i just say hey the message has no slot, 
    //which means its an attack, or there is a slot, meaning its defending

    // if(card_manager_ptr->getMiddle().at(slot).first == NULL){}
    PlayerRole role = IDLE;
    for(auto& pl : players_bs){
        if(pl.first == player_id){
            role = pl.second;
        }
    }
    if(role == ATTACKER || role == CO_ATTACKER){
        for(auto card : cards){
            if(!isValidMove(card, player_id, slot)) return false;
            else attacks_to_defend++;
        }
        attack(); // calls attack function
        return true;
    }
    else{ //defending only 1 card at a time
        if(isValidMove(cards.at(0), player_id, slot)) {
            attacks_to_defend--;
            return true;
        }
    }


    return false;
}

bool Battle::handleActionEvent(){
    //this is going to be a big one
    return false;
}

bool Battle::successfulDefend(){
    //fetches middle from the cardmanager, loops over the middle checks if all the attacks have been
    //defended, or we do
    if(attacks_to_defend == 0){
        // ----------> send message
        return true;
    }
    return false;
}

/**
    *PRE: 
    *POST: it shifts the player battle states and sends message to client and returns true
 */
bool Battle::passOn(/*unsigned player_id*/Card card, int player_id, CardSlot slot){
    //check if isValidMove() -> there cannot be anything defended
    //                       -> the defender has to lay down the card on a free card slot
    //                       -> card has to be valid (i.e same number)

    //getPlayerHand(player_id) check if a card with the same number is in the hand
    //if no return invalid move
    //if yes set attacker to idle, defender to attacker, coattacker to defender, the next player to coattacker
    // if(isValidMove(card, player_id, slot)) 

    //if the next player after player_id is the first_attacker then the first_attacker pointer moves one
    //in the direction of the game

    /*

    p1 --> p2
    |      |
    p4 <-- p3
    
    */
    return false;
}

/**
    *PRE: const Card &card, int player_id, message?
    *POST: returns boolean if the move is valid or not

    *QUESTIONS: should it also pass the message? how does it know if its attacker/defender/idle
                does it check only one card at the time or also multiple if there are multiple that are
                being played? 

 */ 
//informs server that a player is trying to play a card -> specific slot?
// struct PlayCardEvent : public Message {
//     PlayCardEvent();
//     void getContent(rapidjson::Value &content, Allocator &allocator) const;
//     void fromJson(const rapidjson::Value& obj);

//     std::list<Card> cards; // can be multiple if multiple cards are played at once, max 4
//     CardSlot slot; //place of the card
// };
bool Battle::isValidMove( const Card &card, int player_id, CardSlot slot){
    //open socket in isvalid move so we can send the messages to the client
    Network::openSocket(42069); //server side
    //initialize the error message which will be sent if an invalid move is found
    IllegalMoveNotify err_message;

    //if its an attacker
    //check if its the first card being played? if yes check if only one card is played
    //if yes then true
    //else if its two or more cards at the same time check that all of the cards are the same number
    //else if the card is the same number as one of the cards in the middle then it is ok to play

    //set the role per default to idle and it will return false if this is not changed
    int role = IDLE;

    //get the role of the player that is trying to play the card
    for(auto& player : players_bs ){
        if(player.first == player_id){
            role = player.second;
            break;
        }
    }
    if(role == DEFENDER){
        //fetch middle from cardmanager 
        std::vector<std::pair<Card, Card>> middle = card_manager_ptr->getMiddle();
        Card first = middle[slot % 6].first;
        
        //check the slot, if there is a valid card, if its empty return false
        if(first.suit == SUIT_NONE || first.rank == RANK_NONE){
            //notify the illegal move
            err_message.error = "Illegal move: 'empty slot'";
            std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);
            Network::sendMessage(em, player_id);
            return false;
        }

        //check if the card is higher with card_compare
        else if(card_manager_ptr->compareCards(first, card)){
            return true;
        }

        else {
            //notify the illegal move
            err_message.error = "Illegal move";
            std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);
            Network::sendMessage(em, player_id);
            return false;
        } 
    }
    if(role == ATTACKER || role == CO_ATTACKER){
        if(curr_attacks == max_attacks){
            err_message.error = "Illegal move: 'the maximum amount of attacks is already reached'";
            std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);
            Network::sendMessage(em, player_id);
            return false; //idk about this maybe should be > and if == true
        }
        if(curr_attacks == 0){
            return true;
        }
        //check if card rank is in middle
        if(curr_attacks < max_attacks){
            //fetch middle if the card is in play
            std::vector<std::pair<Card, Card>> middle = card_manager_ptr->getMiddle();
            for(auto& card_in_middle : middle){
                if(card_in_middle.first.rank == card.rank || card_in_middle.second.rank == card.rank){
                    return true;
                }
            } 
        }
    }
    
    if(role == IDLE){
        return false;
    }

    return false;
}

void Battle::attack(){
    //calls attack
}

void Battle::defend(){ 
    //calls defendCard
}

// (\(\ 
// ( -.-)
// o_(")(")