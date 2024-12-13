#include "../include/game.hpp"
#include "../include/battle.hpp"
#include "../include/card_manager.hpp"

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
        std::cout << "determined attacker: " << first_attacker << std::endl;
        if(first_attacker == -1){
            // no one has a trump, choose a random player as the first attacker
            first_attacker = rand() % players.size();

            //debugging
            std::cout << "random determined first attacker" << std::endl;
        }
        // ClientID first_defender = (first_attacker + 1) % players.size();
        // ClientID second_attacker = (first_attacker + 2) % players.size();

        auto it = players.find(first_attacker);
        it++;
        if(it == players.end()) it = players.begin();
        ClientID first_defender = *it;
        std::cout << "determined defender: " << first_defender << std::endl;

        it = players.find(first_defender);
        it++;
        if(it == players.end()) it = players.begin();
        ClientID second_attacker = *it;
        std::cout << "determined second attacker: " << second_attacker << std::endl;


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
    // - Start the first battle
    // the constructor of Battle will then communicate to the clients the roles of the players
    current_battle_ = Battle(BATTLETYPE_FIRST, player_roles_, card_manager_, finished_players_);
}


void Game::createBattle(){
    // What does need to happen when a new battle is created?
        // - Check if the game is over
        // - Check if the game is started
        // - Check if the game is in the endgame
        // - Check if the game is in the reset state
        // - Check if the turn order needs to be updated
        // - Check if a client action event needs to be handled
        // - Check if a client card event needs to be handled
        // - Create a new battle

        // determines at what stage the game is (BattleType) and creates battles accordingly
        // if only two players remain it should create an endgame battle
        // if(card_manager_.getNumberActivePlayers() == 2){
        //     current_battle_ = Battle(BATTLETYPE_ENDGAME, player_roles_, card_manager_, finished_players_);
        //     return;
        // } else {
        //     current_battle_ = Battle(BATTLETYPE_NORMAL, player_roles_, card_manager_, finished_players_);
        //     return;
        // }

        //check who has no cards, mark them as Finished in the player roles and 
        updateTurnOrder();
        unsigned count = 0;
        for(auto i : player_roles_){
            if(i.second == ATTACKER || i.second == DEFENDER || i.second == CO_ATTACKER){
                count++;
            }
        }
        if(count <= 2){
            current_battle_ = Battle(BATTLETYPE_ENDGAME, player_roles_, card_manager_, finished_players_);
        }
        else{
            current_battle_ = Battle(BATTLETYPE_NORMAL, player_roles_, card_manager_, finished_players_);
        }
    return;
}

bool Game::isStarted(){
    return false;
}

// i dont think we need this since handlePlayerCardEvent will tell when the game is over (/the durak is found)
//check if game is ended
// bool Game::endGame(){
//     //only one player has cards left in his hand
//     // if(card_manager_.getNumberActivePlayers() == 1){
//     //     return true;
//     // }
//     unsigned count = 0;
//     for(auto c : player_roles_){
//         unsigned int player_hand = card_manager_.getPlayerHand(c.first).size();
//         if(player_hand == 0){
//             count++; //count how many players have 0 cards
//         }
//     }
//     if(card_manager_.getNumberOfCardsOnDeck() == 0 && count == player_roles_.size() - 1){
//         return true;
//     }

//     return false;
// }

bool Game::resetGame(){
    return false;
}

void Game::updateTurnOrder() {
    // Exit early if there are still cards in the deck
    if (card_manager_.getNumberOfCardsOnDeck()) {
        return;
    }

    // Find and update roles
    for (auto it = player_roles_.begin(); it != player_roles_.end(); ++it) {
        // Skip players who are already finished
        if (it->second == FINISHED) {
            continue;
        }

        // If the current ATTACKER has no cards left
        if (it->second == ATTACKER && !card_manager_.getNumberOfCardsInHand(it->first)) {
            it->second = FINISHED; // Mark the current ATTACKER as finished

            // Find the next valid player to assign as ATTACKER
            auto next_it = std::next(it);
            while (next_it != player_roles_.end() && next_it->second == FINISHED) {
                ++next_it;
            }

            // Wrap around if necessary
            if (next_it == player_roles_.end()) {
                next_it = player_roles_.begin();
                while (next_it->second == FINISHED) {
                    ++next_it;
                }
            }

            // Assign the next valid player as ATTACKER
            if (next_it != player_roles_.end()) {
                next_it->second = ATTACKER;

                // Assign the DEFENDER and CO_ATTACKER roles based on the new ATTACKER
                auto defender_it = std::next(next_it);
                while (defender_it != player_roles_.end() && defender_it->second == FINISHED) {
                    ++defender_it;
                }
                if (defender_it == player_roles_.end()) {
                    defender_it = player_roles_.begin();
                    while (defender_it->second == FINISHED) {
                        ++defender_it;
                    }
                }
                defender_it->second = DEFENDER;
                unsigned count = 0;
                for(auto i : player_roles_){
                    if(i.second == ATTACKER || i.second == DEFENDER || i.second == CO_ATTACKER){
                        count++;
                    }
                }
                // if(count <= 2) return;

                auto co_attacker_it = std::next(defender_it);
                while (co_attacker_it != player_roles_.end() && co_attacker_it->second == FINISHED) {
                    ++co_attacker_it;
                }
                if (co_attacker_it == player_roles_.end()) {
                    co_attacker_it = player_roles_.begin();
                    while (co_attacker_it->second == FINISHED) {
                        ++co_attacker_it;
                    }
                }
                co_attacker_it->second = CO_ATTACKER;

                //TODO: need to loop over the rest and set to idle
            }
            return; // Exit after updating roles
        }
    }
}


bool Game::handleClientActionEvent(std::unique_ptr<Message> message, ClientID client){
    if(current_battle_.has_value()){
        ClientActionEvent* return_cacte = dynamic_cast<ClientActionEvent*>(message.get());

        ClientAction action = return_cacte->action;
        
        current_battle_->handleActionEvent(client, action);

        if(current_battle_->battleIsDone()){
            player_roles_ = current_battle_->getPlayerRolesMap();
            current_battle_.reset();
            updateFinishedPlayers();
        }
    }

    return false;
}

// Finds the PlayerID of the last Player
// If there are multiple players left, the function will fail
ClientID Game::findlastplayer(){
    ClientID lastplayer = -1;
    unsigned count = 0;
    for(auto i : player_roles_){
        if(i.second != FINISHED){
            lastplayer = i.first;
            ++count;
        }
    }
    assert(count == 1 && "There should only be one player left");
    assert(lastplayer != -1 && "No player found");
    return lastplayer;
}

// Handles the card event from the client
// Returns 0 if it successfully passed on the event
// If only last player is remaining, the function returns the ClientID of the last player
ClientID Game::handleClientCardEvent(std::unique_ptr<Message> message, ClientID client){
    if(current_battle_.has_value()){
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

        if(card_manager_.getNumberActivePlayers()==1){
            //TODO: Message an client schicke wer durak isch
            ClientID durak = findlastplayer();
            return durak;
            //TODO: Spiel beende
        }
    } else {

        createBattle();
        PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(message.get());

        //calls game function handleClientCardEvent();
        std::vector<Card> vector_of_cards;
        for(auto card : return_pce->cards){
            vector_of_cards.push_back(card);
        }
        CardSlot slot = return_pce->slot;

        current_battle_->handleCardEvent(vector_of_cards, client, slot);
    }
return 0;
}


// After a card has been played this function checks if a player has finished the game
// If that is the case, the player is removed from the player_bs_ map and added to the finished_players set
void Game::updateFinishedPlayers(){

    if (card_manager_.getNumberOfCardsOnDeck()){
        return;
    }
    for (auto it = player_roles_.begin();it != player_roles_.end();){
        if (card_manager_.playerFinished(it->first)){
            finished_players_.insert(it->first);
            it = player_roles_.erase(it);
            
        }
        else{
            ++it;
        }
    }
}
