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
        for(auto i : player_ids){
            std::vector<Card> hand = card_manager_->getPlayerHand(i);
            // iterate through hand
            for(unsigned j = 0; j < hand.size(); j++){
                if(hand[j].suit == trump && hand[j].rank <= current_lowest_trump.rank){
                    current_lowest_trump = hand[j];
                    first_attacker = i;
                }
            }
        }
        std::cout << "determined attacker: " << first_attacker << std::endl;
        if(first_attacker == -1){
            // no one has a trump, choose a random player as the first attacker
            first_attacker = rand() % player_ids.size();

            //debugging
            std::cout << "random determined first attacker" << std::endl;
        }
        // ClientID first_defender = (first_attacker + 1) % player_ids.size();
        // ClientID second_attacker = (first_attacker + 2) % player_ids.size();

        ClientID first_defender = -1; //no player determined
        for(size_t i = 0; i < player_ids.size(); ++i){
            if(player_ids[i] == first_attacker){
                if( i + 1 < player_ids.size()){
                    first_defender = player_ids[i + 1]; 
                }
                else{
                    first_defender = player_ids[0];
                }
                break;
            }
        }
        std::cout << "determined defender: " << first_defender << std::endl;

        ClientID second_attacker = -1; //no player determined

        for(size_t i = 0; i < player_ids.size(); ++i){
            if(player_ids[i] == first_defender){
                if( i + 1 < player_ids.size()){
                    second_attacker = player_ids[i + 1];
                }
                else{
                    second_attacker = player_ids[0];
                }
                break;
            }
        }
        std::cout << "determined second attacker: " << second_attacker << std::endl;


    // set private member player_roles_
    for(auto i : player_ids){
        if(i == first_attacker){
            player_roles_[i] = ATTACKER;
        }
        else if(i == first_defender){
            player_roles_[i] = DEFENDER;
        }
        else if(i == second_attacker){
            player_roles_[i] = CO_ATTACKER;
        }
        else{
            player_roles_[i] = IDLE;
        }
    }
    // - Start the first battle
    // only decomment this when constructor of battle user map
    current_battle_ = new Battle(true, player_roles_, *card_manager_);
    // the constructor of Battle will then communicate to the clients the roles of the players
}

// destructor
Game::~Game(){
    delete card_manager_;
    delete current_battle_;
}

bool Game::createBattle(){
    // What does need to happen when a new battle is created?
        // - Check if the game is over
        // - Check if the game is started
        // - Check if the game is in the endgame
        // - Check if the game is in the reset state
        // - Check if the turn order needs to be updated
        // - Check if a client action event needs to be handled
        // - Check if a client card event needs to be handled
        // - Create a new battle
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

bool Game::handleClientCardEvent(std::unique_ptr<Message> message, ClientID client){
    if(current_battle_ != nullptr){
        // there is a battle and we need to handle the card event
        // current_battle_->handleCardEvent(message, client);
        PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());

        //calls game function handleClientCardEvent();
        std::vector<Card> vector_of_cards;
        for(auto card : return_pce->cards){
            vector_of_cards.push_back(card);
        }
        CardSlot slot = return_pce->slot;

        current_battle_->handleCardEvent(vector_of_cards, client, slot);
    }
    return false;
}
