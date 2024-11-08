#include "../include/battle.hpp"
#include <iostream>


/**
* QUESTIONS: 
 */

//constructor, passes if it is first battle or not and passes the players with their roles
Battle::Battle(bool first_battle, std::vector<std::pair<int, PlayerRole>> players) : 
                                    first_battle_(first_battle), players_bs(players), curr_attacks(0){
    if(first_battle_ == true){
        max_attacks = 5;
    }
    else{
        max_attacks = 6;
    }
};

bool Battle::handleCardEvent(/* the message play card event */){

    std::cout << "handleCardEvent was called" << std::endl;

    //calls isvalidmove and gives the message parameters with the function,
    //here we should handle the list and maybe break it down? by card, then we can call isValidMove multiple 
    //times and check if each card is valid, and if yes, then we give to go
    
    return false;
}

bool Battle::handleActionEvent(){
    return false;
}

bool Battle::successfulDefend(){
    return false;
}

/**
    *PRE: 
    *POST:
 */
bool Battle::passOn(/*unsigned player_id*/){
    //check if isValidMove() -> there cannot be anything defended
    //                       -> the defender has to lay down the card on a free card slot
    //                       -> card has to be valid (i.e same number)

    //getPlayerHand(player_id) check if a card with the same number is in the hand
    //if no return invalid move
    //if yes set attacker to idle, defender to attacker, coattacker to defender, the next player to coattacker
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
bool Battle::isValidMove( const Card &card, int player_id /*message with slots?*/){

    //if its a defender -> how to check that?
    //call cards in middle 
    //find out if the card played on the specific slot is bigger than the one the defender wants to play
    // compare cards function?

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
        
    }
    if(role == ATTACKER){
        if(curr_attacks == max_attacks){
            return false;
        }
        if(curr_attacks == 0 /*&& message -> */){
            return true;
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