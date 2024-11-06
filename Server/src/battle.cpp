#include "../include/battle.hpp"
#include <iostream>


/**
* QUESTIONS: 
 */

//constructor 
Battle::Battle(bool first_battle) : first_battle_(first_battle){
    if(first_battle_ == true){
        max_attacks = 5;
    }
    else{
        max_attacks = 6;
    }
}
bool Battle::handleCardEvent(/* the message card event */){

    std::cout << "handleCardEvent was called" << std::endl;

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
bool Battle::isValidMove( /*const Card &card, int player_id*/){

    //if its a defender -> how to check that?
    //call cards in middle 
    //find out if the card played on the specific slot is bigger than the one the defender wants to play
    // compare cards function?

    //if its an attacker
    //check if its the first card being played? if yes check if only one card is played
    //if yes then true
    //else if its two or more cards at the same time check that all of the cards are the same number
    //else if the card is the same number as one of the cards in the middle then it is ok to play

    return false;
}

void Battle::attack(){
    //calls attack
}

void Battle::defend(){ 
    //calls defendCard
}