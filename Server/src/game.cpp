#include "../include/game.hpp"
#include "../include/battle.hpp"
#include "../include/card_manager.hpp"

Game::Game(std::vector<ClientID> player_ids){
    // What does need to happen when a game of durak is created?
        // - Create a deck of 52 cards              //
        // - Shuffle the deck                       //
        // - Determine the trump suit               // 
        // - Distribute 6 cards to each player      // all done in the constructor of the card_manager
        card_manager_ = new CardManager(player_ids);
    // - Determine the first attacker
        Suit trump = card_manager_->getTrump();
        Card current_lowest_trump = Card(RANK_ACE, trump);
        // iterate through all players and find the one with the lowest trump card
        ClientID first_attacker = -1; // -1 means no one has a trump
        for(unsigned i = 0; i < players_.size(); i++){
            std::vector<Card> hand = card_manager_->getPlayerHand(i);
            // iterate through hand
            for(unsigned j = 0; j < hand.size(); j++){
                if(hand[j].suit == trump && hand[j].rank <= current_lowest_trump.rank){
                    current_lowest_trump = hand[j];
                    first_attacker = i;
                }
            }
        }
        if(first_attacker == -1){
            // no one has a trump, choose a random player as the first attacker
            first_attacker = rand() % players_.size();
        }
    // set private members
    // - Start the first battle
    // Battle first_battle = Battle(first_attacker, players_, card_manager_);

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
