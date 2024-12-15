#include "../include/game.hpp"
#include "../include/battle.hpp"
#include "../include/card_manager.hpp"
#include "../include/server.hpp"


Game::Game(std::set<ClientID> &players) : card_manager_(players) {
    // What does need to happen when a game of durak is created?
        // - Create a deck of 52 cards              //
        // - Shuffle the deck                       //
        // - Determine the trump suit               // 
        // - Distribute 6 cards to each player      // all done in the constructor of the card_manager
        // - Determine the first attacker
        Suit trump = card_manager_.getTrump();
        Card current_lowest_trump = Card(RANK_ACE, trump);
        // iterate through all players and find the one with the lowest trump card
        ClientID first_attacker = -1; // -1 means no one has a trump
        for(auto i : players){
            std::vector<Card> hand = card_manager_.getPlayerHand(i);
            // iterate through hand
            for(unsigned j = 0; j < hand.size(); j++){
                if(hand[j].suit == trump && hand[j].rank <= current_lowest_trump.rank){
                    current_lowest_trump = hand[j];
                    first_attacker = i;
                }
            }
        }
        std::cout << "Determined attacker: " << first_attacker << std::endl;
        if(first_attacker == -1){
            // no one has a trump, choose a random player as the first attacker
            first_attacker = rand() % players.size();

            // debugging
            std::cout << "Random determined first attacker: " << first_attacker << std::endl;
        }

        // determine the defender
        auto it = players.find(first_attacker);
        it++;
        if(it == players.end()) it = players.begin();
        ClientID first_defender = *it;
        std::cout << "Determined defender: " << first_defender << std::endl;

        // determine the second attacker
        it = players.find(first_defender);
        it++;
        if(it == players.end()) it = players.begin();
        ClientID second_attacker = *it;
        std::cout << "Determined second attacker: " << second_attacker << std::endl;


    // set private member player_roles_
    for(auto i : players){
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

    // Start the first battle
    // the constructor of Battle will then communicate to the clients the roles of the players
    current_battle_ = Battle(BATTLETYPE_FIRST, player_roles_, card_manager_, finished_players_);
}


void Game::createBattle(){
    // What does need to happen when a new battle is created?
        // - Check if the game is in the endgame
        // - Create a new battle

    // determines at what stage the game is (BattleType) and creates battles accordingly
    // if only two players remain it should create an endgame battle
    if(card_manager_.getNumberActivePlayers() == 2){
        current_battle_ = Battle(BATTLETYPE_ENDGAME, player_roles_, card_manager_, finished_players_);
        return;
    } else {
        current_battle_ = Battle(BATTLETYPE_NORMAL, player_roles_, card_manager_, finished_players_);
        return;
    }
}


void Game::handleClientActionEvent(std::unique_ptr<Message> message, ClientID client){

    // if there is a game pass on the action event to the battle
    if(current_battle_.has_value()){

        ClientActionEvent* return_cacte = dynamic_cast<ClientActionEvent*>(message.get());
        ClientAction action = return_cacte->action;
        current_battle_->handleActionEvent(client, action);

        // if the battle is done, update the player roles and reset the battle
        if(current_battle_->battleIsDone()){
            player_roles_ = current_battle_->getPlayerRolesMap();
            current_battle_.reset();
        }

    } else {
        std::cerr << "No active game to handle action event." << std::endl;
    }
    return;
}


ClientID Game::findlastplayer(){
    ClientID lastplayer = -1;
    unsigned count = 0;
    for(auto i : current_battle_->getPlayerRolesMap()){
        if(i.second != FINISHED){
            lastplayer = i.first;
            ++count;
        }
    }
    assert(count == 1 && "There should only be one player left");
    assert(lastplayer != -1 && "No player found");
    return lastplayer;
}


ClientID Game::handleClientCardEvent(std::unique_ptr<Message> message, ClientID client){
    // if there is a battle and we need to handle the card event
    if(current_battle_.has_value()){

        PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());

        // calls game function handleClientCardEvent();
        std::vector<Card> vector_of_cards;
        for(auto card : return_pce->cards){
            vector_of_cards.push_back(card);
        }
        CardSlot slot = return_pce->slot;

        current_battle_->handleCardEvent(vector_of_cards, client, slot);

        // checks if this move could end the game by succesfully defending with last card
        if(current_battle_->isgameover()){
            std::cout << "GAMEOVER" << std::endl;
            ClientID durak = findlastplayer();
            isgameover_ = true;
            // the message informing all clients who is durak is sent in msg_handler.cpp after this function returns
            return durak;
        }

    } else {

        createBattle();
        PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());

        // calls game function handleClientCardEvent();
        std::vector<Card> vector_of_cards;
        for(auto card : return_pce->cards){
            vector_of_cards.push_back(card);
        }
        CardSlot slot = return_pce->slot;

        current_battle_->handleCardEvent(vector_of_cards, client, slot);

        // ckecks if this move could end the game by succesfully attacking with last card
        if(current_battle_->isgameover()){
            std::cout << "GAMEOVER" << std::endl;
            ClientID durak = findlastplayer();
            isgameover_ = true;
            // the message informing all clients who is durak is sent in msg_handler.cpp after this function returns
            return durak;
        }
    }
return 0;
}


bool Game::isgameOver(){
    return isgameover_;
}