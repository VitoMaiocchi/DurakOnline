#include "../include/game.hpp"
#include "../include/battle.hpp"
#include "../include/card_manager.hpp"
// #include <__config>

Game::Game(std::vector<ClientID> player_ids){
    // What does need to happen when a game of durak is created?
        // - Create a deck of 52 cards              //
        // - Shuffle the deck                       //
        // - Determine the trump suit               // 
        // - Distribute 6 cards to each player      // done in the constructor of the card_manager
        card_manager = new CardManager(players_bs);
    // - Determine the first attacker
    // - Start the first battle
    

}

bool Game::makeFirstBattle(){
    return false;
}

bool Game::createBattle(){
    return false;
}

bool Game::isStarted(){
    return false;
}

bool Game::endGame(){
    return false;
}

bool Game::resetGame(){
    return false;
}

bool Game::updateTurnOrder(){
    return false;
}

bool Game::handleClientActionEvent(){
    return false;
}

bool Game::handleClientCardEvent(){
    return false;
}
