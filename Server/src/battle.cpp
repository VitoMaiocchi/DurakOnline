#include "../include/battle.hpp"


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
Battle::Battle(bool first_battle, std::map<ClientID, PlayerRole> players, CardManager &card_manager) : 
                                    first_battle_(first_battle), players_bs_(players), card_manager_ptr_(&card_manager) 
                                    ,curr_attacks_(0){
    
    max_attacks_ = first_battle ? 5 : 6;

    BattleStateUpdate bsu_msg;
    //set the first attacker pointer to the one that attacks first
    //while iterating prepare the message BattleStateUpdate to send to the client
    for(auto& pl : players_bs_){
        if(pl.second == ATTACKER){
            first_attacker_ = &pl;
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == DEFENDER){
            bsu_msg.defender = pl.first;
        }
        else if(pl.second == CO_ATTACKER){
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == IDLE){
            bsu_msg.idle.push_back(pl.first);
        }
        std::cout << "Debugging purposes: " << pl.first << ": " << pl.second << std::endl;
    }

    for(auto& pl : players_bs_){
        Network::sendMessage(std::make_unique<BattleStateUpdate>(bsu_msg), pl.first); //maybe make function to broadcast to all
    }

    for(auto& pl : players_bs_){
        AvailableActionUpdate update;
        if(pl.second == ATTACKER){
            update.ok = true;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == DEFENDER){
            update.ok = false;
            update.pass_on = true;
            update.pick_up = true;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == CO_ATTACKER){
            update.ok = true;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == IDLE){
            update.ok = false;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }

    }



};

//default dtor
Battle::~Battle() = default;

bool Battle::handleCardEvent(std::vector<Card> cards, ClientID player_id, CardSlot slot){

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

    //attacker or coattacker
    if(role == ATTACKER || role == CO_ATTACKER){
        //if only 1 card with which is being attacked, check if valid move
        if(cards.size() == 1 && isValidMove(cards.at(0), player_id, slot)){
            //if valid move then attack with this card
            attack(player_id, cards.at(0));

            return true;
        }
        //else if more than one card is being played at the same time
        //also checks for if the max_attacks are reached
        else if(cards.size() > 1 && (cards.size() + curr_attacks_ <= max_attacks_)){
            for(size_t i = 0; i < cards.size() - 1; ++i){
                if(cards[i].rank != cards[i + 1].rank){
                    IllegalMoveNotify err_msg;
                    err_msg.error = "Illegal Move: 'the selected cards do not match in rank'";
                    Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_msg), player_id);
                    return false;
                }
                else {attacks_to_defend_++;}
                std::cout << "attacks to defend: " << attacks_to_defend_ <<std::endl;
            }
            return true;
        }
    }
    else if(role == DEFENDER && cards.size() == 1){ //defending only 1 card at a time
        //we need to check which slot the defender is placing the card
        //if the slot is empty then it means defender is trying to pass the attack on
        //this is only possble when:
        /**
         * defender hasn't defended any other cards yet
         * defender has a valid card & places said valid card to pass on
         * edge case: defender places two cards to pass_on
         */
        std::vector<std::pair<std::optional<Card>,std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
        if(!middle[slot % 6].first.has_value()){
            //now we know the slot is empty, now we need to call pass_on()
            passOn(cards.at(0), player_id, slot);
            //check if the middle has been updated
            middle = card_manager_ptr_->getMiddle(); //get it again
            if(middle[slot % 6].first.has_value() && 
               middle[slot % 6].first->rank == cards[0].rank && 
               middle[slot % 6].first->suit == cards[0].suit){
                return true;
            }
            // return false;
        } 
        if(isValidMove(cards.at(0), player_id, slot)) {
            defend(player_id, cards.at(0), slot); //this function causes an address boundary error
            return true;
        }
    }
    else if(role == DEFENDER && defense_started_){
        if(isValidMove(cards.at(0), player_id, slot)) {
            defend(player_id, cards.at(0), slot); //this function causes an address boundary error
            return true;
        }
    }


    return false;
}
/**
 * PRE: takes the message (already broken down)
 * POST: calls the next functions, either pick_up or pass_on, returns true if this succeeded
 */
bool Battle::handleActionEvent(ClientID player_id, ClientAction action){

    std::vector<Card> hand = card_manager_ptr_->getPlayerHand(player_id); 
    Suit trump = card_manager_ptr_->getTrump();
    std::vector<std::pair<std::optional<Card>, std::optional<Card>>> field = card_manager_ptr_->getMiddle();

    //pass on -> check for trump -> if valid card we can passOn() but without playing the card
    if(action == CLIENTACTION_PASS_ON && players_bs_[player_id] == DEFENDER){
        if(defense_started_){ //cannot pass the attack on if already started defending
            //send illegal action notification
            IllegalMoveNotify notify;
            notify.error = "Illegal move: 'Cannot pass the attack on if already started defending'";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(notify), player_id);

            return false;
        }
        //for every card in the middle i want to test the whole hand if there is a card that is trump
        //and that matches the rank of the card, if yes then we can say break and then call 
        //moveplayer roles, but if not then if it reaches the end of the middle vector without finding
        //anything it should send an IllegalNotify message = illegal action
        bool valid = false;
        for(auto slot : field){
            for(auto card : hand){
                if(card.suit == trump){
                    if(card.rank == slot.first->rank){
                        //correct
                        valid = true;
                        break;
                    }
                }
            }
        }
        if(valid){
            movePlayerRoles(); //moves player roles one to the right
            return true;
        }
        else{
            IllegalMoveNotify err_msg;
            err_msg.error = "Illegal move: 'Cannot pass on with your cards'";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_msg), player_id);
        }
    }


    //pick up
    else if(action == CLIENTACTION_PICK_UP){
        //check if theres at least one card not defended -> flag defended unnecessary? or maybe function
        // for(auto& slot : field){
        //     if(slot.first != empty_card_ && slot.second == empty_card_){
        //         card_manager_ptr_->pickUp(player_id);
        //         return true;
        //     }
        // }
        if(!successfulDefend()){
            card_manager_ptr_->pickUp(player_id);
            return true;
        }
        if(successfulDefend()){
            //notify with illegal move (studid, why pick up when you defended everything lol)
            return false;
        }

        return false;
    }

    //ok
    else if(action == CLIENTACTION_OK){
        //what should happen here? 
        return true;
    }
    
    return false;
}

bool Battle::successfulDefend(){
    //fetches middle from the cardmanager, loops over the middle checks if all the attacks have been
    //defended, or we do
    std::vector<std::pair<std::optional<Card>, std::optional<Card>>> field = card_manager_ptr_->getMiddle();
    for(const auto slot : field){
        if(slot.first.has_value() && !slot.second.has_value()){
            return false;
        }
    }
    if(attacks_to_defend_ == 0){
        // ----------> send message
        return true;
    }
    return true;
}

/**
    *PRE: the defender clicks his card/cards and then an empty slot on the battlefield
    *POST: it shifts the player battle states and sends message to client and returns true
 */
bool Battle::passOn(Card card, ClientID player_id, CardSlot slot){
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
    if(!isValidMove(card, player_id, slot)){
        return false; //if the move is not valid
    }
    else {
        //moves player roles one up/next
        movePlayerRoles();
        //player_bs_[player_id] is now attacker
        attack(player_id, card);
        //check if everything worked
        std::vector<std::pair<std::optional<Card>, std::optional<Card>>> middle = card_manager_ptr_->getMiddle();
        for(auto slotM : middle){
            if(slotM.first == card && players_bs_[player_id] == ATTACKER){
                return true;
            }
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
    IllegalMoveNotify err_message;

    //if its an attacker
    //check if its the first card being played? if yes check if only one card is played
    //if yes then true
    //else if its two or more cards at the same time check that all of the cards are the same number
    //else if the card is the same number as one of the cards in the middle then it is ok to play

    //check if the card is in the players hand 
    const std::vector<Card>& player_hand = card_manager_ptr_->getPlayerHand(player_id);
    if(std::find(player_hand.begin(), player_hand.end(), card) == player_hand.end()){
        err_message.error = "Illegal move: 'card was not found in your hand'";
        Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
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
                err_message.error = "Illegal move: 'Cannot place a card on an empty slot when defending'";
                Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
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

        //check if the card is higher with card_compare
        else if(card_manager_ptr_->compareCards(*first, card)){
            return true;
        }

        else {
            std::cout << "ERROR MESSAGE: Illegal move: compare cards is not correct" <<std::endl;
            //notify the illegal move
            err_message.error = "Illegal move";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
            return false;
        } 
    }
    if(role == ATTACKER){
        if(curr_attacks_ == max_attacks_){
            err_message.error = "Illegal move: 'the maximum amount of attacks is already reached'";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
            return false; //idk about this maybe should be > and if == true
        }
        if(curr_attacks_ == 0){
            return true;
        }
        //check if card rank is in middle
        if(curr_attacks_ < max_attacks_){
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
        if(curr_attacks_ == max_attacks_){
            err_message.error = "Illegal move: 'the maximum amount of attacks is already reached'";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
            return false; //idk about this maybe should be > and if == true
        }
        if(curr_attacks_ == 0){
            err_message.error = "Illegal move: 'First Attacker hasnt attacked yet'";
            Network::sendMessage(std::make_unique<IllegalMoveNotify>(err_message), player_id);
            return false;
        }
        //check if card rank is in middle
        if(curr_attacks_ < max_attacks_){
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

    return false;
}

void Battle::attack(ClientID client, Card card){
    //calls attack
    card_manager_ptr_->attackCard(card, client);
    attacks_to_defend_++;
    curr_attacks_++;
    std::cout << "attacks to defend: " << attacks_to_defend_ <<std::endl;

}

void Battle::defend(ClientID client, Card card, CardSlot slot){ 
    //calls defendCard
    card_manager_ptr_->defendCard(card, client, slot);
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

/**
 * POST: moves the player roles one to the next 
 */
void Battle::movePlayerRoles(){
    std::cout << "moving the player roles" << std::endl;
    PlayerRole last_value = players_bs_.rbegin()->second;

    for(auto it = players_bs_.rbegin(); it != players_bs_.rend(); ++it){
        if(std::next(it) != players_bs_.rend()){
            it->second = std::next(it)->second;
        }
    }
    players_bs_.begin()->second = last_value;
    //send the battle state updates
    BattleStateUpdate bsu_msg;
    //set the first attacker pointer to the one that attacks first
    //while iterating prepare the message BattleStateUpdate to send to the client
    for(auto& pl : players_bs_){
        if(pl.second == ATTACKER){
            first_attacker_ = &pl;
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == DEFENDER){
            bsu_msg.defender = pl.first;
        }
        else if(pl.second == CO_ATTACKER){
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == IDLE){
            bsu_msg.idle.push_back(pl.first);
        }
        std::cout << "Debugging purposes: " << pl.first << ": " << pl.second << std::endl;
    }

    for(auto& pl : players_bs_){
        Network::sendMessage(std::make_unique<BattleStateUpdate>(bsu_msg), pl.first); //maybe make function to broadcast to all
    }
    //set the first attacker pointer to the one that attacks first
    //while iterating prepare the message BattleStateUpdate to send to the client
    for(auto& pl : players_bs_){
        if(pl.second == ATTACKER){
            first_attacker_ = &pl;
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == DEFENDER){
            bsu_msg.defender = pl.first;
        }
        else if(pl.second == CO_ATTACKER){
            bsu_msg.attackers.push_back(pl.first);
        }
        else if(pl.second == IDLE){
            bsu_msg.idle.push_back(pl.first);
        }
        std::cout << "Debugging purposes: " << pl.first << ": " << pl.second << std::endl;
    }

    for(auto& pl : players_bs_){
        Network::sendMessage(std::make_unique<BattleStateUpdate>(bsu_msg), pl.first); //maybe make function to broadcast to all
    }

    //send the new available action updates
    for(auto pl : players_bs_){
        AvailableActionUpdate update;
        if(pl.second == ATTACKER){
            update.ok = true;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == DEFENDER){
            update.ok = false;
            update.pass_on = true;
            update.pick_up = true;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == CO_ATTACKER){
            update.ok = true;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }
        if(pl.second == IDLE){
            update.ok = false;
            update.pass_on = false;
            update.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update), pl.first);
        }

    }
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
// (\(\ 
// ( -.-)
// o_(")(")