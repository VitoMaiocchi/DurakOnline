#include "../include/game.hpp"
#include <__config>

Game::Game(std::vector<std::pair<int, PlayerRole>> players_bs){
    // What does need to happen when a game of durak is created?
        // done in the constructor of the card_manager
            // - Create a deck of 52 cards
            // - Shuffle the deck
            // - Determine the trump suit
            // - Distribute 6 cards to each player
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
