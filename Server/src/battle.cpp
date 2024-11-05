#include "battle.hpp"
#include <iostream>


/**
* QUESTIONS: 
 */
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
bool Battle::passOn(){
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

void Battle::attack(){}

void Battle::defend(){}