#include "../include/battle.hpp"
#include "Networking/util.hpp"
#include "../include/server.hpp"


/**
* QUESTIONS: do i need a game pointer?
* TODO: - implement a check for how many cards are already in middle (handleCardEvent)
        - send messages AvailableActionUpdate
        - send messages BattleStateUpdate when pass_on is called
        - save the first attacker and who all played cards for the card manager
        - implement handleActionEvent()
        - test pass on function
 */

//constructor, passes if it is first battle or not and passes the players with their roles
Battle::Battle(BattleType btype, std::map<ClientID, PlayerRole> players, CardManager &card_manager, std::set<ClientID> finished_players) : 
                                    btype_(btype), players_bs_(players), card_manager_ptr_(&card_manager),
                                    finished_players_(finished_players), curr_attacks_(0){
    
        //DEBUG
    std::cout <<"\nnumber of players in player_bs at start of battle = " << players_bs_.size() << "\n" << std::endl; 
    std::string s = "";
    switch(btype_) {
        case BATTLETYPE_FIRST:
            s = "FIRST BATTLE";
            first_battle_ = true;
            break;
        case BATTLETYPE_NORMAL:
            s = "NORMAL BATTLE";
            first_battle_ = false;
            break;
        case BATTLETYPE_ENDGAME:
            s = "ENDGAME BATTLE";
            first_battle_ = false;
            // here it should set a flag so that if the second to last player plays his last card/s
            // then the battle and game should end
            move_could_end_game_ = true;
            break;
        default:
            first_battle_ = false;
            break;
    }

    std::cout << "\n\nBTYPE: " << s << "\n\n" << std::endl;
    //END DEBUG
    
    std::cout << "CREATE NEW BATTLE" << std::endl;
    phase_ = BATTLEPHASE_FIRST_ATTACK;

    // max_attacks_ = first battle ? 5 : 6;
    if(first_battle_){
        max_attacks_ = 5;
    }
    //if in the endgame, where players have less than 6 cards on hand
    else if(card_manager_ptr_->getPlayerHand(getCurrentDefender()).size() < 6){
        max_attacks_ = card_manager_ptr_->getPlayerHand(getCurrentDefender()).size();
    }
    else{
        max_attacks_ = 6;
    }
    battle_done_ = false;

    BattleStateUpdate bsu_msg;
    //set the first attacker pointer to the one that attacks first
    //while iterating prepare the message BattleStateUpdate to send to the client
    // we first iterate through to set the first attacker
    for(ClientID c : DurakServer::clients){
        if(players_bs_.find(c) != players_bs_.end()){
            if(players_bs_[c] == ATTACKER){
                std::pair<const ClientID, PlayerRole> attacker(c, players_bs_[c]);
                first_attacker_ = &attacker;
                bsu_msg.attackers.push_front(c);
                attack_order_.push_front(c);
            }else if(players_bs_[c] == DEFENDER){
                bsu_msg.defender = c;
            }else if(players_bs_[c] == CO_ATTACKER){
                bsu_msg.attackers.push_back(c);
                attack_order_.push_back(c);
            }else if(players_bs_[c] == IDLE || players_bs_[c] == FINISHED){
                bsu_msg.idle.push_back(c);
            }
        }
        else{
            bsu_msg.idle.push_back(c);
        }
    }
    // for(auto& pl : players_bs_){
    //     if(pl.second == ATTACKER){
    //         first_attacker_ = &pl;
    //         bsu_msg.attackers.push_front(pl.first);
    //         attack_order_.push_front(pl.first);
    //         //send the normal action update
    //     }
    // }
    // then we iterate through another time to ensure the co attacker is appended to the list and does not come in front of the attacker
    for(auto& pl : players_bs_){
        // if(pl.second == CO_ATTACKER){
        //     bsu_msg.attackers.push_back(pl.first);
        //     attack_order_.push_back(pl.first);
        //     //send the normal action update
        // }
        // else if(pl.second == DEFENDER){
        //     bsu_msg.defender = pl.first;
        //     //send the normal action update
        // }
        // else if(pl.second == IDLE || pl.second == FINISHED){
        //     bsu_msg.idle.push_back(pl.first);
        //     //send the normal action update
        // }
        std::cout << "Debugging purposes: id: " << pl.first << ": role" << pl.second << std::endl;
    }

    // //the finished players are automatically just observers
    // for(ClientID f : finished_players_){
    //     bsu_msg.idle.push_back(f);
    // }

    for(ClientID c : DurakServer::clients){
        Network::sendMessage(std::make_unique<BattleStateUpdate>(bsu_msg), c); //maybe make function to broadcast to all
    }

    updateAvailableAction();

};

//default dtor
Battle::~Battle() = default;

void sendPopup(std::string message, ClientID clientID) {
    PopupNotify popup;
    popup.message = message;
    Network::sendMessage(std::make_unique<PopupNotify>(popup), clientID);
}

void Battle::sendBattleStateUpdate(){
    if(players_bs_.size() == 0) return;

    BattleStateUpdate bsu_msg;
    bsu_msg.defender = 0;
    //set the first attacker pointer to the one that attacks first
    //while iterating prepare the message BattleStateUpdate to send to the client
    // we first iterate through to set the first attacker
    for(ClientID c : DurakServer::clients){
        if(players_bs_.find(c) != players_bs_.end()){
            if(players_bs_[c] == ATTACKER){
                bsu_msg.attackers.push_front(c);
            }else if(players_bs_[c] == DEFENDER){
                bsu_msg.defender = c;
            }else if(players_bs_[c] == CO_ATTACKER){
                bsu_msg.attackers.push_back(c);
            }else if(players_bs_[c] == IDLE || players_bs_[c] == FINISHED){
                bsu_msg.idle.push_back(c);
            }
        } else{
            bsu_msg.idle.push_back(c);
        }
    }

    //dont send if there is no defender left (this is the case at the end of the game)
    if(bsu_msg.defender == 0) return; 

    for(ClientID c : DurakServer::clients){
        Network::sendMessage(std::make_unique<BattleStateUpdate>(bsu_msg), c); //maybe make function to broadcast to all
    }
}


void broadcastPopup(std::string message) {
    PopupNotify popup;
    popup.message = message;
    for(ClientID client : DurakServer::clients) {
        Network::sendMessage(std::make_unique<PopupNotify>(popup), client);
    }
}

std::string getClientName(ClientID clientID){
    return DurakServer::players_map[clientID].name;
}

void Battle::attackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot) {
    //if only 1 card with which is being attacked, check if valid move
    if(cards.size() == 1 && isValidMove(cards.at(0), player_id, slot) && !pickUp_){
        //if valid move then attack with this card
        attack(player_id, cards.at(0));
        // sendAvailableActionUpdate(0, player_id);
        // sendAvailableActionUpdate(0, getCurrentDefender());
        phase_ = BATTLEPHASE_OPEN;
        return;
    }
    //else if more than one card is being played at the same time
    //also checks for if the max_attacks are reached
    // tp place multiple cards we need to check if they exist in the middle
    else if(cards.size() > 1 && (cards.size() + curr_attacks_ <= max_attacks_) && !pickUp_){
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> field = card_manager_ptr_->getMiddle();
        
        //we will iterate through the middle with std::find
        if(curr_attacks_ > 0){
            Rank targetrank = cards[0].rank;
            auto it = std::find_if(field.begin(), field.end(), 
                    [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                        return (pair.first.has_value() && pair.first->rank == targetrank) || 
                            (pair.second.has_value() && pair.second->rank == targetrank);
                    });
            if(it == field.end()){
                PopupNotify err_msg;
                err_msg.message = "Illegal Move: 'the selected cards do not match in rank'";
                Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
                return;
            }
        


            for(size_t i = 1; i < cards.size(); ++i){

                //we will iterate through the middle with std::find
                Rank targetrank = cards[i].rank;
                auto it = std::find_if(field.begin(), field.end(), 
                        [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                            return (pair.first.has_value() && pair.first->rank == targetrank) || 
                                (pair.second.has_value() && pair.second->rank == targetrank);
                        });
                // BUG: break this if statement up
                if(/*cards[i].rank != cards[i - 1].rank &&*/ it == field.end()){
                    PopupNotify err_msg;
                    err_msg.message = "Illegal Move: 'the selected cards do not match in rank'";
                    Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
                    return;
                }
            }
        }
        else if(curr_attacks_ == 0){
            for(size_t i = 1; i < cards.size(); ++i){
                if(cards[i].rank != cards[i - 1].rank){
                    PopupNotify err_msg;
                    err_msg.message = "Illegal Move: 'the selected cards do not match in rank'";
                    Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
                    return;
                }
            }
        }
        //now place all attacks on the field
        for(size_t i = 0; i < cards.size(); ++i){
            attack(player_id, cards.at(i));
            phase_ = BATTLEPHASE_OPEN;
        }
        return;
    }
    //to throw in cards after the defender has picked them up
    else if(phase_ == BATTLEPHASE_POST_PICKUP && !ok_msg_[ATTACKER]){
        if(cards.size() + curr_attacks_ <= max_attacks_){
            for(size_t i = 0; i < cards.size(); ++i){
                Rank targetrank = cards[i].rank;
                auto it = std::find_if(picked_up_cards_.begin(), picked_up_cards_.end(),
                    [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                        return  (pair.first.has_value() && pair.first->rank == targetrank) || 
                            (pair.second.has_value() && pair.second->rank == targetrank);
                    });
                if(it == picked_up_cards_.end()){
                    //send illegal move msg
                    return;
                }
            }
            //place the cards but without calling attack from battle
            //we call attack from card manager and then pick up for the defender
            for(size_t i = 0; i < cards.size(); ++i){
                card_manager_ptr_->attackCard(cards[i], player_id);
                card_manager_ptr_->pickUp(getCurrentDefender());
            }

        }
    }
    else if(ok_msg_[ATTACKER]){
        //illegal move msg
        return;
    }
}

void Battle::coAttackerCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot) {
    //coattacker can only start attacking once the attacker placed at least one card
    //we just check the amount of current attacks 
    if(curr_attacks_ == 0) {
        PopupNotify err_msg;
        err_msg.message = "Illegal Move: 'You are only co-attacker and you cannot start the attack'";
        Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
        return;
    }

    if(curr_attacks_ > 0 && !pickUp_ && cards.size() == 1){
        //if only one card is being attacked with
        if(isValidMove(cards.at(0), player_id, slot)){
            //if valid move then attack with this card
            attack(player_id, cards.at(0));
            phase_ = BATTLEPHASE_OPEN;

            return;
        }
    }
    //else if more than one card is being played at the same time
    //also checks for if the max_attacks are reached
    // tp place multiple cards we need to check if they exist in the middle
    else if(cards.size() > 1 && (cards.size() + curr_attacks_ <= max_attacks_) && !pickUp_){
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> field = card_manager_ptr_->getMiddle();
        
        //we will iterate through the middle with std::find
        Rank targetrank = cards[0].rank;
        auto it = std::find_if(field.begin(), field.end(), 
                [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                    return (pair.first.has_value() && pair.first->rank == targetrank) || 
                        (pair.second.has_value() && pair.second->rank == targetrank);
                });
        if(it == field.end()){
            PopupNotify err_msg;
            err_msg.message = "Illegal Move: 'the selected cards do not match in rank'";
            Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
            return;
        }


        for(size_t i = 1; i < cards.size(); ++i){

            //we will iterate through the middle with std::find
            Rank targetrank = cards[i].rank;
            auto it = std::find_if(field.begin(), field.end(), 
                    [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                        return (pair.first.has_value() && pair.first->rank == targetrank) || 
                            (pair.second.has_value() && pair.second->rank == targetrank);
                    });

            if(it == field.end()){
                PopupNotify err_msg;
                err_msg.message = "Illegal Move: 'the selected cards do not match in rank'";
                Network::sendMessage(std::make_unique<PopupNotify>(err_msg), player_id);
                return;
            }
        }
        //now place all attacks on the field
        for(size_t i = 0; i < cards.size(); ++i){
            attack(player_id, cards.at(i));
            phase_ = BATTLEPHASE_OPEN;
        }
        return;
    }
    else if(phase_ == BATTLEPHASE_POST_PICKUP && !ok_msg_[CO_ATTACKER]){
        if(cards.size() + curr_attacks_ <= max_attacks_){
            for(size_t i = 0; i < cards.size(); ++i){
                Rank targetrank = cards[i].rank;
                auto it = std::find_if(picked_up_cards_.begin(), picked_up_cards_.end(),
                    [&targetrank](const std::pair<std::optional<Card>,std::optional<Card>>& pair){
                        return  (pair.first.has_value() && pair.first->rank == targetrank) || 
                            (pair.second.has_value() && pair.second->rank == targetrank);
                    });
                if(it == picked_up_cards_.end()){
                    //send illegal move msg
                    std::cerr << "not correct" <<std::endl;
                    return;
                }
            }
            //place the cards but without calling attack from battle
            //we call attack from card manager and then pick up for the defender
            for(size_t i = 0; i < cards.size(); ++i){
                card_manager_ptr_->attackCard(cards[i], player_id);
                card_manager_ptr_->pickUp(getCurrentDefender());
            }

        }
    }
    else if(ok_msg_[CO_ATTACKER]){
        //illegal move msg
        return;
    }
}

//TODO: das sött rank si
bool checkIdenticalSuit(std::unordered_set<Card> &cards, ClientID clientID) {
    const Suit suit = cards.begin()->suit;
    for(Card c : cards) if(c.suit != suit) {
        sendPopup("Can not play cards of different suits", clientID);
        return true;
    }
    return false;
}
bool checkIdenticalRank(std::unordered_set<Card> &cards, ClientID clientID){
    const Rank rank = cards.begin()->rank;
    for(Card c : cards)if(c.rank != rank){
        sendPopup("Can not play cards of different rank", clientID);
        return true;
    }
    return false;

}

bool Battle::topSlotsClear() {
    for(uint i = 6; i<12; i++) if(card_manager_ptr_->getMiddleSlot(i).has_value()) return false;
    return true;
}

bool Battle::passOnRankMatch(Rank rank) {
    for(uint i = 0; i<6; i++) {
        if(!card_manager_ptr_->getMiddleSlot(i).has_value()) continue;
        if(card_manager_ptr_->getMiddleSlot(i).value().rank != rank) return false;
    }
    return true;
}

void Battle::defenderCardEvent(std::unordered_set<Card> &cards, ClientID clientID, CardSlot slot) {
    switch (phase_) {
        case BATTLEPHASE_FIRST_ATTACK:
            sendPopup("Can not place card; Waiting for first attack.", clientID);
            return;
        case BATTLEPHASE_DONE:
            return;
        case BATTLEPHASE_POST_PICKUP:
            sendPopup("You allready picked up. You can not place any more cards", clientID);
            return;
        case BATTLEPHASE_DEFENDED:
            sendPopup("Everything is defended. Can not place any more cards", clientID);
            return;
        case BATTLEPHASE_OPEN:
            break;
    } //BATTLE STATE OPEN:

    uint s = (uint) slot;
    if(card_manager_ptr_->getMiddleSlot(s%6).has_value()) { //SLOT is not empty
        if(cards.size() > 1) {
            sendPopup("you can only defend with one card per slot", clientID);
            return;
        }
        if(isValidMove(*cards.begin(), clientID, slot)) {
            defend(clientID, *cards.begin(), slot);
            if(successfulDefend()) phase_ = BATTLEPHASE_DEFENDED;
        }
        else sendPopup("invalid defend", clientID);
        return;
    }

    //Slot is empty: try to pass on
    if(checkIdenticalRank(cards, clientID)) return;

    if(!topSlotsClear()) {
        sendPopup("you allready defended a card. Pass on is not possilbe", clientID);
        return;
    }

    if(!passOnRankMatch(cards.begin()->rank)) {
        sendPopup("Can not pass on: Ranks don't match", clientID);
        return;
    }

    if(first_battle_) {
        sendPopup("Can not pass on: First battle", clientID);
        return;
    }
    
    //TODO: das gaht nur mit einere karte...
    passOn(cards, clientID, slot);
    phase_ = BATTLEPHASE_OPEN;
}

bool Battle::handleCardEvent(std::vector<Card> &cards, ClientID player_id, CardSlot slot){
    if(cards.size() == 0) return false;

    std::cout << "handleCardEvent was called" << std::endl;

    
    //calls isvalidmove and gives the message parameters with the function,
    //here we should handle the list and maybe break it down? by card, then we can call isValidMove multiple 
    //times and check if each card is valid, and if yes, then we give to go


    //set initial player role to IDLE
    PlayerRole role = IDLE;

    //check if the key exists in the map
    if (players_bs_.find(player_id) == players_bs_.end()) {
        std::cerr << "player id not found in the role vector" << std::endl;
        return false; // or handle error
    }
    //access the role with the key player_id
    role = players_bs_[player_id];

    auto card_set = std::unordered_set<Card>(cards.begin(), cards.end());

    //attacker or coattacker
    switch (role) {
        case ATTACKER:
            attackerCardEvent(cards, player_id, slot);
            break;
        case CO_ATTACKER:
            coAttackerCardEvent(cards, player_id, slot);
            break;
        case DEFENDER:
            defenderCardEvent(card_set, player_id, slot);
            break;
        case IDLE:
            sendPopup("You are idle and can not place cards", player_id);
            break;
        case FINISHED:
            sendPopup("You are finished and can not place cards", player_id);
            break;
    }
    sendBattleStateUpdate();
    updateAvailableAction();
    card_manager_ptr_->cardUpdate();
    return false;
}

void Battle::tryPickUp() {
    switch(btype_){
        case BATTLETYPE_FIRST:
        case BATTLETYPE_NORMAL:
            if(ok_msg_[ATTACKER] && ok_msg_[CO_ATTACKER]){
                card_manager_ptr_->pickUp(getCurrentDefender());
                card_manager_ptr_->clearMiddle();
                card_manager_ptr_->distributeNewCards(first_attacker_->first, players_bs_);
                movePlayerRoles();
                movePlayerRoles(); //loses right to attack when picking up

                //das isch alles chli goofy
                curr_attacks_ = 0;
                ok_msg_[ATTACKER] = false;
                ok_msg_[CO_ATTACKER] = false;
                phase_ = BATTLEPHASE_FIRST_ATTACK;
                first_battle_ = false;
                battle_done_ = true;
            }
            return;
        case BATTLETYPE_ENDGAME:
            if(ok_msg_[ATTACKER]){
                card_manager_ptr_->pickUp(getCurrentDefender());
                card_manager_ptr_->clearMiddle();
                card_manager_ptr_->distributeNewCards(first_attacker_->first, players_bs_);
                movePlayerRoles(); //loses right to attack when picking up
                //why only called once?
                //das isch alles chli goofy
                curr_attacks_ = 0;
                ok_msg_[ATTACKER] = false;
                phase_ = BATTLEPHASE_FIRST_ATTACK;
                first_battle_ = false;
                battle_done_ = true;
            }
        default:
            std::cout << "tryPickUp called in wrong phase" << std::endl;
            return;
    }
}
void sendReadyUpdate(ClientID client){
    ReadyUpdate rupdate;
    rupdate.players.insert(client);
    Network::sendMessage(std::make_unique<ReadyUpdate>(rupdate), client);
}
void Battle::doneEvent(ClientID clientID) {

    if(players_bs_[clientID] == ATTACKER)    {ok_msg_[ATTACKER] = true; sendReadyUpdate(clientID);}
    if(players_bs_[clientID] == CO_ATTACKER) {ok_msg_[CO_ATTACKER] = true; sendReadyUpdate(clientID);}
    if(btype_ == BATTLETYPE_NORMAL || btype_ == BATTLETYPE_FIRST) {
        if(phase_ == BATTLEPHASE_DEFENDED && ok_msg_[ATTACKER] && ok_msg_[CO_ATTACKER]) {
            card_manager_ptr_->clearMiddle();
            card_manager_ptr_->distributeNewCards(first_attacker_->first, players_bs_);
            movePlayerRoles(); 
            
            //das isch alles chli goofy
            curr_attacks_ = 0;
            ok_msg_[ATTACKER] = false;
            ok_msg_[CO_ATTACKER] = false;
            phase_ = BATTLEPHASE_FIRST_ATTACK;
            if(BATTLETYPE_NORMAL) first_battle_ = false;
            else if(BATTLETYPE_FIRST) first_battle_ = true;
            battle_done_ = true;

            return;
        }
        if(phase_ == BATTLEPHASE_POST_PICKUP) tryPickUp();
    } else if(btype_ == BATTLETYPE_ENDGAME){
        std::cout << "doneEvent of endgame" << std::endl;
        switch(phase_){
            case BATTLEPHASE_DEFENDED:
                if(ok_msg_[ATTACKER]){
                    card_manager_ptr_->clearMiddle();
                    card_manager_ptr_->distributeNewCards(first_attacker_->first, players_bs_);
                    movePlayerRoles();
                    sendBattleStateUpdate(); // inform all clients of their new roles
                    //das isch alles chli goofy
                    curr_attacks_ = 0;
                    ok_msg_[ATTACKER] = false;
                    phase_ = BATTLEPHASE_FIRST_ATTACK;
                    first_battle_ = false;
                    battle_done_ = true;
                    return;
                } else {
                    sendPopup("Co-Attacker cannot press Done in 1v1", clientID);
                }
            case BATTLEPHASE_POST_PICKUP:
                tryPickUp();
                card_manager_ptr_->clearMiddle();
                card_manager_ptr_->distributeNewCards(first_attacker_->first, players_bs_);
                movePlayerRoles();
                //das isch alles chli goofy
                curr_attacks_ = 0;
                ok_msg_[ATTACKER] = false;
                phase_ = BATTLEPHASE_FIRST_ATTACK;
                first_battle_ = false;
                battle_done_ = true;
                return;
            default:
                sendPopup("some done event functionality missing", clientID);
                return;
        }
    }
}

std::optional<Card> Battle::getReflectCard(ClientID clientID) {
    if(players_bs_[clientID] != DEFENDER) return std::nullopt;
    if(first_battle_) return std::nullopt;
    if(!topSlotsClear()) return std::nullopt;
    
    std::vector<Card> hand = card_manager_ptr_->getPlayerHand(clientID); 
    Suit trump = card_manager_ptr_->getTrump();
    for(Card card : hand) {
        if(card.suit != trump) continue;
        if(!passOnRankMatch(card.rank)) continue;
        return card;
    }

    return std::nullopt;
}

void Battle::reflectEvent(ClientID clientID) {
    auto card = getReflectCard(clientID);
    std::cout << "reflect event" << std::endl;
    if(!card.has_value()) return;
    std::cout << "reflect event succesfull" << std::endl;
    movePlayerRoles();
    UpdatePickUpOrder();
}

void Battle::pickupEvent(ClientID clientID) {
    if(players_bs_[clientID] != DEFENDER || phase_ != BATTLEPHASE_OPEN) return;
    phase_ = BATTLEPHASE_POST_PICKUP;
    tryPickUp();
}

/**
 * PRE: takes the message (already broken down)
 * POST: calls the next functions, either pick_up or pass_on
 */
void Battle::handleActionEvent(ClientID player_id, ClientAction action){
    std::string message = "";
    switch(action){
        case CLIENTACTION_PASS_ON:
            reflectEvent(player_id);
            break;
        case CLIENTACTION_OK:
            std::cout << "clientaction ok in handleactionevent" << std::endl;
            doneEvent(player_id);
            message = (player_id) + " pressed done.";
            broadcastPopup(message);
            break;
        case CLIENTACTION_PICK_UP:
            pickupEvent(player_id);
            message = getClientName(player_id) + " picked up. You can now throw in or press done.";
            for(auto it : players_bs_) {
                if(it.second == ATTACKER || it.second == CO_ATTACKER) {
                    sendPopup(message, it.first);    
                }
            }
            break;
        case CLIENTACTION_READY:
            std::cerr << "ready message should not reach into battle" << std::endl;
            break;
        default:
            std::cerr << "unknown action" << std::endl;
            break;
    }
    sendBattleStateUpdate();
    updateAvailableAction();
    card_manager_ptr_->cardUpdate();
    return;
}

void Battle::updateAvailableAction() {
    //DEBUG
    std::string s = "";
    switch(phase_) {
        case BATTLEPHASE_FIRST_ATTACK:
            s = "FIRST ATTACK";
        break;
        case BATTLEPHASE_POST_PICKUP:
            s = "POST PICKUP";
        break;
        case BATTLEPHASE_OPEN:
            s = "OPEN";
        break;
        case BATTLEPHASE_DEFENDED:
            s = "DEFENDED";
        break;
        case BATTLEPHASE_DONE:
            s = "DONE";
    }

    std::cout << "\n\nPHASE: " << s << "\n\n" << std::endl;
    //END DEBUG

    for(auto bs : players_bs_) {
        const ClientID id = bs.first;
        const PlayerRole role = bs.second;

        AvailableActionUpdate update;
        update.ok = false;
        if(phase_ == BATTLEPHASE_POST_PICKUP || phase_ == BATTLEPHASE_DEFENDED) {
            if(role == ATTACKER && !ok_msg_[ATTACKER]) update.ok = true;
            if(role == CO_ATTACKER && !ok_msg_[CO_ATTACKER]) update.ok = true;
        }
        
        update.pass_on = false;
        if(role == DEFENDER && phase_ == BATTLEPHASE_OPEN) 
            if(getReflectCard(id).has_value()) update.pass_on = true;
        
        update.pick_up = false;
        if(role == DEFENDER && phase_ == BATTLEPHASE_OPEN)
            update.pick_up = true;
        
        Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), id);
    }
}

bool Battle::successfulDefend(){
    for(uint i = 0; i<6; i++)
        if(card_manager_ptr_->getMiddleSlot(i).has_value() 
            && !card_manager_ptr_->getMiddleSlot(i+6).has_value())
                return false;
    return true;

    // //fetches middle from the cardmanager, loops over the middle checks if all the attacks have been
    // //defended, or we do
    // std::vector<std::pair<std::optional<Card>, std::optional<Card>>> field = card_manager_ptr_->getMiddle();
    // for(const auto slot : field){
    //     if(slot.first.has_value() && !slot.second.has_value()){
    //         return false;
    //     }
    // }
    // if(attacks_to_defend_ == 0){
    //     // ----------> send message ok = true, pick up = false, pass on = false
    //     sendAvailableActionUpdate(2, getCurrentDefender());
    //     return true;
    // }
    // return true;
}

/**
    *PRE: the defender clicks his card/cards and then an empty slot on the battlefield
    *POST: it shifts the player battle states and sends message to client and returns true
 */
bool Battle::passOn(std::unordered_set<Card>& cards, ClientID player_id, CardSlot slot){
    //check if isValidMove() -> there cannot be anything defended
    //                       -> the defender has to lay down the card on a free card slot
    //                       -> card has to be valid (i.e same number)

    //getPlayerHand(player_id) check if a card with the same number is in the hand
    //if no return invalid move
    //if yes set attacker to idle, defender to attacker, coattacker to defender, the next player to coattacker
    // if(isValidMove(card, player_id, slot)) 

    //if the next player after player_id is the first_attacker_ then the first_attacker_ pointer moves one
    //in the direction of the game

    /*

    p1 --> p2
    |      |
    p4 <-- p3
    
    */
    // players_bs_[player_id] this guy is still defender currently

    
    //check if valid move
    //this should happen before the player roles are moved
    if(!isValidMove(*cards.begin(), player_id, slot) || curr_attacks_ == max_attacks_){
        // PopupNotify popup;
        // popup.message = "Illegal Move: "
        return false; //if the move is not valid
    }
    if(cards.size() == 1){
        if(!isValidMove(*cards.begin(), player_id, slot) || curr_attacks_ == max_attacks_){
            // PopupNotify popup;
            // popup.message = "Illegal Move: "
            return false; //if the move is not valid
        }
    }
    if(cards.size() > 1){
        for(auto c : cards){
            if(!isValidMove(c, player_id, slot) || curr_attacks_ + cards.size() == max_attacks_){
                return false;
            }
        }
    }

        //moves player roles one up/next
        movePlayerRoles();
        //This function should only be calles when there are 3 or more active players
        if(card_manager_ptr_->getNumberActivePlayers() >= 3){
            UpdatePickUpOrder();
        }

        //player_bs_[player_id] is now attacker
        for(auto& c : cards){
            attack(player_id, c);
        }

        //check if everything worked
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
        for(auto slotM : middle){
            if(slotM.first == *cards.begin() && players_bs_[player_id] == ATTACKER){
                return true;
            }
        }
     





    return false;
}

/**
    *PRE: const Card &card, int player_id, message?
    *POST: returns boolean if the move is valid or not

    *QUESTIONS: should it also pass the message? how does it know if its attacker/defender/idle
                does it check only one card at the time or also multiple if there are multiple that are
                being played? 

 */ 
bool Battle::isValidMove( const Card &card, ClientID player_id, CardSlot slot){
    std::cout << "isValidMove() was called" << std::endl;
    //initialize the error message which will be sent if an invalid move is found
    PopupNotify err_message;

    //if its an attacker
    //check if its the first card being played? if yes check if only one card is played
    //if yes then true
    //else if its two or more cards at the same time check that all of the cards are the same number
    //else if the card is the same number as one of the cards in the middle then it is ok to play

    //get the defenders hand to check how many cards can be played, this is a second check to ensure
    //when an attack was passed on that the defender can defend all cards
    std::size_t defender_card_amount = card_manager_ptr_->getPlayerHand(getCurrentDefender()).size();
    //check if the card is in the players hand 
    const std::vector<Card>& player_hand = card_manager_ptr_->getPlayerHand(player_id);
    if(std::find(player_hand.begin(), player_hand.end(), card) == player_hand.end()){
        err_message.message = "Illegal move: 'card was not found in your hand'";
        Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
        std::cout << "the card was not found in the players hand" << std::endl;

        //for debugging purposes we will print the player hand and the card we try to access
        std::cout << "the card we try to play: " << card.rank << "-" << card.suit << std::endl;
        std::cout << "player hand:  ";
        for(Card c : player_hand){
            std::cout << c.rank << "-" << c.suit << "  ";
        }
        std::cout << std::endl;
        return false; // card not found in the hand
    }

    //set the role per default to idle and it will return false if this is not changed
    int role = IDLE;

    //get the role of the player that is trying to play the card
    role = players_bs_[player_id];

    //if the player is defender
    if(role == DEFENDER){
        //fetch middle from cardmanager 
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
        std::cout << "slot: " << slot << std::endl;
        std::optional<Card> first = middle[slot % 6].first;
        
        //check the slot, if its empty and the defense was already started return false
        if(!first.has_value()){

            // the card was played on a slot that is empty
            if(defense_started_){
                std::cout << "ERROR MESSAGE: Illegal move: empty slot" <<std::endl;
                //notify the illegal move
                err_message.message = "Illegal move: 'Cannot place a card on an empty slot when defending'";
                Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
                return false;
            }
            else{
                std::cout << "TRYING TO PASS THE ATTACK ON" << std::endl;
                //need checks here
                for(const auto s : middle){
                    if(s.first->rank == card.rank){
                        std::cout << "passing on is valid" << std::endl;
                        return true;
                    }
                }

            }

        }
        else if(middle[slot % 6].second.has_value()){
            sendPopup("The card has already been defended", player_id);
            return false;
        }
        //check if the card is higher with card_compare
        else if(card_manager_ptr_->compareCards(*first, card)){
            return true;
        }

        else {
            std::cout << "ERROR MESSAGE: Illegal move: compare cards is not correct" <<std::endl;
            //notify the illegal move
            err_message.message = "Illegal move";
            Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
            return false;
        } 
    }
    if(role == ATTACKER){
        //set max attacks to the amount of cards in defenders hand
        // max_attacks_ = card_manager_ptr_->getNumberOfCardsInHand(getCurrentDefender()) < 6 ? card_manager_ptr_->getNumberOfCardsInHand(getCurrentDefender()) : 6;
        if(curr_attacks_ == max_attacks_ || 0 == defender_card_amount){
            err_message.message = "Illegal move: 'the maximum amount of attacks is already reached'";
            Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
            return false; //idk about this maybe should be > and if == true
        }
        if(curr_attacks_ == 0){
            return true;
        }
        //check if card rank is in middle
        if(curr_attacks_ < max_attacks_ && 0 < defender_card_amount){
            //fetch middle if the card is in play
            std::vector<std::pair<std::optional<Card>, std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
            for(auto card_in_middle : middle){
                if(card_in_middle.first->rank == card.rank || card_in_middle.second->rank == card.rank){
                    return true;
                }
            } 
        }
    }
    //coattacker can only jump in on the attack after the attacker started attcking
    if(role == CO_ATTACKER){
        // max_attacks_ = card_manager_ptr_->getNumberOfCardsInHand(getCurrentDefender()) < 6 ? card_manager_ptr_->getNumberOfCardsInHand(getCurrentDefender()) : 6;
        if(curr_attacks_ == max_attacks_ || 0 == defender_card_amount){
            err_message.message = "Illegal move: 'the maximum amount of attacks is already reached'";
            Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
            return false; //idk about this maybe should be > and if == true
        }
        if(curr_attacks_ == 0){
            err_message.message = "Illegal move: 'First Attacker hasnt attacked yet'";
            Network::sendMessage(std::make_unique<PopupNotify>(err_message), player_id);
            return false;
        }
        //check if card rank is in middle
        if(curr_attacks_ < max_attacks_ && 0 < defender_card_amount){
            //fetch middle if the card is in play
            std::vector<std::pair<std::optional<Card>, std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
            for(auto card_in_middle : middle){
                if(card_in_middle.first->rank == card.rank || card_in_middle.second->rank == card.rank){
                    return true;
                }
            } 
        }
    }
    
    if(role == IDLE){
        return false;
    }
    std::cerr << "no case found that is legal" << std::endl;
    return false;
}

void Battle::attack(ClientID client, Card card){
    std::cout << "attack() was called"<<std::endl;
    //calls attack
    card_manager_ptr_->attackCard(card, client);
    // if there are only two players left (and thus a move could end the game) and the attacker has no cards left, the game should end
    if( (card_manager_ptr_->getPlayerHand(client).size() == 0) && move_could_end_game_){
        std::cout << "game should end by attacking" << std::endl;
        players_bs_[client] = FINISHED;
        gameover_ = true;
    }

    attacks_to_defend_++;
    curr_attacks_++;
    std::cout << "attacks to defend: " << attacks_to_defend_ <<std::endl;
    if(ok_msg_[ATTACKER]) ok_msg_[ATTACKER] = false;
    if(ok_msg_[CO_ATTACKER]) ok_msg_[CO_ATTACKER] = false;
    
    //check for other players to finish during an active battle and set flag move_could_end_game to true
    if(card_manager_ptr_->getNumberActivePlayers() == 2){
        for(auto c : players_bs_){
            if(card_manager_ptr_->getNumberOfCardsInHand(c.first) == 1){
                move_could_end_game_ = true;
            }
        }
    }

    updateAvailableAction();

}

void Battle::defend(ClientID client, Card card, CardSlot slot){ 
    std::cout << "defend was called" <<std::endl;
    //calls defendCard
    card_manager_ptr_->defendCard(card, client, slot);
    // if there are only two players left (and thus a move could end the game) and the attacker has no cards left, the game should end
    if( (card_manager_ptr_->getPlayerHand(client).size() == 0) && move_could_end_game_){
        // the game should end
        std::cout << "game should end by defending" << std::endl;
        players_bs_[client] = FINISHED;
        gameover_ = true;
    }
    attacks_to_defend_--;
    defense_started_ = true;
    std::cout << "attacks to defend: " << attacks_to_defend_ <<std::endl;
}


// irrelevant?
const std::pair<const ClientID, PlayerRole>* Battle::getFirstAttackerPtr(){
    if(first_attacker_ == nullptr){
        std::cerr << "Error: 'first attacker' not found" <<std::endl;
        return nullptr;
    }
    return first_attacker_;
}

std::map<ClientID, PlayerRole>::iterator next(std::map<ClientID, PlayerRole> map, std::map<ClientID, PlayerRole>::iterator it) {
    auto next = std::next(it);
    if(next == map.end()) return map.begin();
    return next;
}

std::map<ClientID, PlayerRole>::iterator previous(std::map<ClientID, PlayerRole> map, std::map<ClientID, PlayerRole>::iterator it) {
    if(it == map.begin()) return std::prev(map.end());
    return std::prev(it);
}

void Battle::removeFinishedPlayers(){
    //TODO: da no checke obs nur no öpper het wo nöd fertig isch oder so ka
    // if(!battle_done_){
    //     std::cout << "postpone until a battle is finished or after pick up"<<std::endl;
    //     return;
    // }
    //Find the player to be removed, return if all players still have cards
    auto no_cards = [this](const auto& pair) {
            return card_manager_ptr_->getPlayerHand(pair.first).empty();};
    auto finished = std::find_if(players_bs_.begin(), players_bs_.end(), no_cards);


    if (finished == players_bs_.end()) {
        // Special handling for exactly 3 players
        if (players_bs_.size() == 3) {
            std::cout << "handling 3 players case in removeFinishedPLayers"<<std::endl;
            auto it = std::find_if(players_bs_.begin(), players_bs_.end(), [](const std::pair<ClientID, PlayerRole>& pair){return pair.second == ATTACKER;});
            if(it == players_bs_.end()){
                std::cout << "Error, missing attacker role for 3 player case" << std::endl;
            }
            it->second = ATTACKER;
            it = nextInOrderIt(it);
            it->second = DEFENDER;
            it = nextInOrderIt(it);
            it->second = CO_ATTACKER; // Third player becomes CO-ATTACKER

            std::cout << "\nthe 3 players when in removeFinishedPLayers roles: \n";
            for(auto c : players_bs_){
                std::cout << "id: " << c.first << " role: " << c.second << "\n";
            }
            std::cout << std::endl;
        }
        if(players_bs_.size() == 2){
            std::cout << "handling 2 players case in removeFinishedPLayers"<<std::endl;
            auto it = std::find_if(players_bs_.begin(), players_bs_.end(), [](const std::pair<ClientID, PlayerRole>& pair){return pair.second == ATTACKER;});
            if(it == players_bs_.end()){
                std::cout << "error attacker not ffound in two player case"<<std::endl;
            }
            it->second = ATTACKER;
            nextInOrderIt(it)->second = DEFENDER;

            std::cout << "\nthe 2 players when in removeFinishedPLayers roles: \n";
            for(auto c : players_bs_){
                std::cout << "id: " << c.first << " role: " << c.second << "\n";
            }
            std::cout << std::endl;
        }
        return;
    }
    
    if(finished->second == ATTACKER) {
        previous(players_bs_, finished)->second = ATTACKER;
        players_bs_.erase(finished);
        std::cout << "made attacker to finshed and removed from players_bs" << std::endl;
        removeFinishedPlayers(); //same function?
        return;
    }

    auto current = finished;
    PlayerRole next_role = next(players_bs_, current)->second;

    while (next_role != ATTACKER){
        next(players_bs_, current)->second = current->second;
        current = next(players_bs_, current);
        next_role = next(players_bs_, current)->second;
    }
    std::cout << "removing finished player: " << finished->first << std::endl;

    players_bs_.erase(finished);


    removeFinishedPlayers();
}

// std::map<ClientID, PlayerRole>::reverse_iterator &non_finished_next(std::map<ClientID, PlayerRole>::reverse_iterator &it) {
//     it++;
//     while(it->second == FINISHED) it++;
//     return it;
// }
/**
 * POST: moves the player roles one to the next 
 */
void Battle::movePlayerRoles(){
     std::cout << "Moving player roles" << std::endl;
    
    removeFinishedPlayers();

        // Handle role rotation for exactly 3 players
    if (players_bs_.size() == 3) {
        auto attacker_it = std::find_if(players_bs_.begin(), players_bs_.end(),
                                        [](const auto& pair) { return pair.second == ATTACKER; });
        auto defender_it = std::find_if(players_bs_.begin(), players_bs_.end(),
                                        [](const auto& pair) { return pair.second == DEFENDER; });
        auto coattacker_it = std::find_if(players_bs_.begin(), players_bs_.end(),
                                          [](const auto& pair) { return pair.second == CO_ATTACKER; });

        if (attacker_it == players_bs_.end() || defender_it == players_bs_.end() || coattacker_it == players_bs_.end()) {
            std::cerr << "Error: Missing required roles when rotating with 3 players." << std::endl;
            return;
        }

        // Rotate roles: ATTACKER -> CO-ATTACKER -> DEFENDER -> ATTACKER
        attacker_it->second = CO_ATTACKER;
        coattacker_it->second = DEFENDER;
        defender_it->second = ATTACKER;

        return;
    }

    PlayerRole end_role = std::prev(players_bs_.end())->second;
    for(auto it = players_bs_.rbegin(); it != std::prev(players_bs_.rend()); it++) {
        it->second = std::next(it)->second;        
    }
    players_bs_.begin()->second = end_role;
    std::cout << "\nRoles updated\n"<<std::endl;
    // BattleStateUpdate bsu_msg; // Prepare message to broadcast role updates

    // // Check the number of active players
    // size_t active_players = 0;
    // for(auto player : players_bs_){
    // if(player.second == ATTACKER || player.second == DEFENDER || player.second == CO_ATTACKER){
    //     active_players++;
    // }
    // }

    // // Handle cases where there are fewer than 3 players left
    // if (btype_ == BATTLETYPE_ENDGAME) {
    //     // Rotate roles between ATTACKER and DEFENDER only
    //     auto attacker_it = std::find_if(players_bs_.begin(), players_bs_.end(),
    //                                     [](const auto& pair) { return pair.second == ATTACKER; });
    //     auto defender_it = std::find_if(players_bs_.begin(), players_bs_.end(),
    //                                     [](const auto& pair) { return pair.second == DEFENDER; });

    //     if (attacker_it != players_bs_.end() && defender_it != players_bs_.end()) {
    //         // Swap their roles
    //         attacker_it->second = DEFENDER;
    //         defender_it->second = ATTACKER;
    //     } else {
    //         std::cerr << "Error: Missing ATTACKER or DEFENDER role in player roles." << std::endl;
    //     }
    //     for(auto player : players_bs_){
    //         if(player.second != ATTACKER && player.second != DEFENDER){
    //             player.second = IDLE;
    //         }
    //     }

    //     // Prepare BattleStateUpdate message
    //     for (const auto& [player_id, role] : players_bs_) {
    //         if (role == ATTACKER) {
    //             bsu_msg.attackers.push_back(player_id);
    //         } else if (role == DEFENDER) {
    //             bsu_msg.defender = player_id;
    //         }
    //         else{
    //             bsu_msg.idle.push_back(player_id);
    //         }
    //     }
    // }
    // // Handle the case where there are 3 or more players
    // else {
    //     // Rotate roles in a circular fashion: ATTACKER -> CO_ATTACKER -> DEFENDER -> IDLE
    //     PlayerRole last_role = players_bs_.rbegin()->second; // Save the last player's role
    //     for (auto it = players_bs_.rbegin(); it != players_bs_.rend(); ++it) {
    //         if (std::next(it) != players_bs_.rend()) {
    //             if(std::next(it)->second == FINISHED) continue;
    //             it->second = std::next(it)->second;
    //         }
    //     }
    //     if(last_role != FINISHED){
    //         players_bs_.begin()->second = last_role; // Assign the last role to the first player
    //     }

    //     // Prepare BattleStateUpdate message

    // }

    sendBattleStateUpdate();

    // Adapt max_attacks to the new defender's card count (min 6 or their card count)
    auto defender_it = std::find_if(players_bs_.begin(), players_bs_.end(),
                                    [](const auto& pair) { return pair.second == DEFENDER; });
    if (defender_it != players_bs_.end()) {
        size_t defender_cards = card_manager_ptr_->getNumberOfCardsInHand(defender_it->first);
        max_attacks_ = std::min<size_t>(defender_cards, 6);
    }
    updateAvailableAction();
}

//getter function for game
bool Battle::battleIsDone(){
    return battle_done_;
}

std::map<ClientID, PlayerRole> Battle::getPlayerRolesMap(){
    return players_bs_;
}

//This function is called when an attack is passed on & Player Roles have already been moved
//it updates the attack order deque
//This function should only be called for when there are 3 or more active players
void Battle::UpdatePickUpOrder(){
    //find client IDs of defender, attacker CoAttacker and first attacker of the battle
    //IDs correspond to Player Roles after they have been moved
    ClientID current_attacker = findRole(ATTACKER);
    ClientID current_defender = findRole(DEFENDER);
    ClientID current_coattacker;
    if(btype_ == BATTLETYPE_ENDGAME) current_coattacker = current_attacker;
    else current_coattacker = findRole(CO_ATTACKER);
    ClientID first_attacker = getFirstAttackerPtr()->first; //First player to attack during this battle
    
    //1a Case: New Defender already was first attacker during this battle
    //Case 1a can only occur when there are either 3 or 4 players left
    if(current_defender == attack_order_.front()){
        attack_order_.pop_front();
        attack_order_.push_back(current_attacker);
    }


    //1b Case: New attacker is the first attacker who started this battle
    //Case 1b can only occur when there are exactly 3 players left
    
    else if (current_attacker == first_attacker) {
        attack_order_.pop_front();                      //remove current defender
        attack_order_.push_front(current_attacker);  //add previous defender who already was an attacker
    }

    //2. Case: New Co-Attacker already was first attacker during this battle
    else if(current_coattacker == first_attacker) {
        attack_order_.pop_back();                       //remove current defender
        attack_order_.push_back(current_attacker);   //add previous defender
        //current coattacker doesn't have to be added, already is in the list
    }


    //3. Case: none of the cases above,
    //New Defender is removed and new attacker & coattacker are added
    else{
        attack_order_.pop_back();                       //remove current defender
        attack_order_.push_back(current_attacker);   //add previous defender
        attack_order_.push_back(current_coattacker); //add current coattacker
    }
    
}

//For any valid ClientID returns the ID of the player who is next
//This function assumes that only "active" players (who haven't finished yet) are in the players_bs_ map
ClientID Battle::nextInOrder(ClientID current_player){
    //Check that map isn't empty
    assert(!players_bs_.empty() && "Map should not be empty here");
    //Create Iterator pointing to current client ID
    auto it = players_bs_.find(current_player);

    //Handle the case where a wrong ClientID was entered
    assert (it != players_bs_.end() && "Invalid PlayerID");
    

    ++it;
    //Check for case where the ClientID was at the end of the map
    if(it == players_bs_.end()){
        it = players_bs_.begin();
    }

    while (it->second == FINISHED){
        ++it;
        if(it == players_bs_.end()){
            it = players_bs_.begin();
        }
    }

    // Otherwise return next element
    return it->first;
}

//Call this function with either ATTACKER, DEFENDER or COATTACKER
//and it returns the corresponding CLientID
//THIS FUNCTION SHOULD ONLY BE CALLED WITH CO_ATTACKER IF THERE ARE 3 OR MORE PLAYERS (it expects the Role to be found)
//THIS FUNCTION SHOULDNT BE CALLED FOR IDLE
ClientID Battle::findRole(PlayerRole role){
    for(const auto& pair : players_bs_){
        if (pair.second == role){
            return pair.first;
        }
    }

    throw std::logic_error("No Player with role found when calling findRole function.");

}

/**
 * POST: returns the current defenders player id
 */
// irrelevant?
ClientID Battle::getCurrentDefender(){
    ClientID player_id = -1;
    for(auto player : players_bs_){
        if(player.second == DEFENDER){
            player_id = player.first;
        }
    }
    return player_id;
}

//Returns an iterator to the next active player in order
std::map<ClientID, PlayerRole>::iterator Battle::nextInOrderIt (std::map<ClientID, PlayerRole>::iterator it){
    // Increment the iterator
    ++it;

    // If the iterator reaches the end, wrap around to the beginning
    if (it == players_bs_.end()) {
        it = players_bs_.begin();
    }

    while (it->second == FINISHED){
        ++it;
        if (it == players_bs_.end()){
            it = players_bs_.begin();
        }
    }
    

    return it;
}
