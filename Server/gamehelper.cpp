#include "gamehelper.hpp"


void fillDeck(State &state){
    using namespace Protocol;
    
    for(Suit suit : {SUIT_CLUBS,SUIT_SPADES,SUIT_DIAMONDS,SUIT_HEARTS}){
        for(Rank rank : {RANK_TWO,RANK_THREE,RANK_FOUR,RANK_FIVE,RANK_SIX,RANK_SEVEN,RANK_EIGHT,
            RANK_NINE,RANK_TEN,RANK_JACK,RANK_QUEEN,RANK_KING,RANK_ACE}){
                state.draw_pile.emplace_back(rank, suit);
        }
    }
}
    
void shuffleCards(State &state){
    auto& deck = state.draw_pile;
    // Check if deck has been initialized properly
    assert(deck.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define pseudo random number generator
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(deck.begin(), deck.end(), g);
}
    
void distributeCardsBeginOfGame(State &state){
    auto& deck = state.draw_pile;
    // distribute cards to player
    for (auto& hand : state.player_hands){
        // Check if players hands are empty
        assert(hand.empty() && "Player's hand should be empty before dealing");
        
        // distribute the top 6 cards to the player
        hand.insert(deck.begin(), deck.begin()+6);
        
        // remove cards from deck
        deck.erase(deck.begin(), deck.begin()+6);
    }
}
    
void determineTrump(State &state){
    auto& deck = state.draw_pile;
    auto& hands = state.player_hands;

    assert(deck.size() == 52 - (6 * hands.size()) 
        && "Deck must contain exactly 52 cards before determining trump");
    
    state.trump_card = deck.back();
}   

void clearMiddle(State &state){
    for(auto &opt : state.middle_cards){
        opt.reset();
    }
}

void drawFromMiddle(Player player, State &state){
    auto& deck = state.draw_pile;
    auto& hands = state.player_hands;

    while(hands[player].size() < 6 && !deck.empty()){ 
        hands[player].insert(deck.front());
        deck.pop_front();
    }
}

uint countCardsInMiddle(State &state){

}

void distributeNewCards(State &state){
    using namespace Protocol;
    auto& roles = state.player_roles;
    auto& count = state.player_count;

    std::vector<int> drawOrder;
    drawOrder.reserve(count);

    /*find the first to draw*/
    int first_idx = 0; // first is currently just the attacker
    auto it = std::find_if(roles.begin(), roles.end(), [](const PlayerRole role){return role == ATTACKER;});
    if(it != roles.end()){
        first_idx = std::distance(roles.begin(), it);
    }
    
    std::iota(drawOrder.begin(), drawOrder.end(), 0);
    std::rotate(drawOrder.begin(), drawOrder.begin() + first_idx, drawOrder.end());

    //move defender to the end
    int def = drawOrder[1];
    drawOrder.erase(drawOrder.begin() + 1);
    drawOrder.push_back(def);

    //for each player draw six cards
    for(int p : drawOrder) drawFromMiddle(p, state);
}

Player findAttacker(State &state){
    using namespace Protocol;
    auto& roles = state.player_roles;

    auto it = std::find_if(roles.begin(), roles.end(), 
    [](PlayerRole role){return role == ATTACKER;});
    Player attacker_idx = -1;
    if(it != roles.end()){
        attacker_idx = std::distance(roles.begin(), it);
    }
    return attacker_idx;
}

Player findDefender(State &state){
    Player defender = (findAttacker(state) + 1) % state.player_count;
    return defender;
}

void movePlayerRoles(State &state){
    using namespace Protocol;
    auto& count = state.player_count;
    auto& roles = state.player_roles;

    //find finished players and save them in the vector
    std::vector<bool> is_finished(count, false); //fixes the finished player roles
    for(Player i = 0; i < count; ++i){
        if(roles[i] == FINISHED){
            is_finished[i] = true;
        }
    }

    //extract the non finished roles
    
    std::vector<PlayerRole> movable;
    movable.reserve(count);
    for(Player i = 0; i < count; ++i){
        if(!is_finished[i]) movable.push_back(roles[i]);
    }

    //rotate the non finished roles
    std::rotate(movable.begin(), movable.end() - 1, movable.end());

    //put them back into the normal player_roles vector
    for(Player i = 0, j = 0; i < count; ++i){
        if(!is_finished[i]){
            roles[i] = movable[j++]; //increment j after inserting role
        }
    }
}

void movePlayerRolesOneBack(State &state){
    using namespace Protocol;
    auto& count = state.player_count;
    auto& roles = state.player_roles;

    //find finished players and save them in the vector
    std::vector<bool> is_finished(count, false); //fixes the finished player roles
    for(Player i = 0; i < count; ++i){
        if(roles[i] == FINISHED){
            is_finished[i] = true;
        }
    }

    //extract the non finished roles
    
    std::vector<PlayerRole> movable;
    movable.reserve(count);
    for(Player i = 0; i < count; ++i){
        if(!is_finished[i]) movable.push_back(roles[i]);
    }

    //rotate to the left
    std::rotate(movable.begin(), movable.begin() + 1, movable.end());

    //put them back into the normal player_roles vector
    for(Player i = 0, j = 0; i < count; ++i){
        if(!is_finished[i]){
            roles[i] = movable[j++]; //increment j after inserting role
        }
    }
}

std::vector<Player> findFinishedPlayers(State &state){
    auto& count = state.player_count;
    auto& hands = state.player_hands;
    auto& roles = state.player_roles;

    assert(count == hands.size() && count == roles.size() && "The sizes and the count must match");
    
    std::vector<Player> finished_players;
    for(std::size_t i = 0; i < count; ++i){
        if(hands[i].empty()){
            finished_players.push_back(static_cast<Player>(i));
        }
    }
    std::sort(finished_players.rbegin(), finished_players.rend()); //descending order for easy erasing
    return finished_players;
}

void setPlayerToFinished(Player player_idx, State &state){
    state.player_roles[player_idx] = Protocol::PlayerRole::FINISHED;
}

void eraseFinishedPlayer(Player player_idx, State &state){
    using namespace Protocol;
    auto& count = state.player_count;
    auto& roles = state.player_roles;

    switch(roles[player_idx]){
        case ATTACKER : {
            movePlayerRoles(state); //attacker becomes idle
            setPlayerToFinished(player_idx, state);
            movePlayerRolesOneBack(state);
            break;
        }
        case DEFENDER : {
            movePlayerRoles(state); //defender becomes attacker
            movePlayerRoles(state); //defender becomes idle
            setPlayerToFinished(player_idx, state);
            movePlayerRolesOneBack(state);
            break;
        }
        case CO_ATTACKER : {
            assert(count >= 3 && "there cannot be a coattacker with less than 3 players");
            if(count == 3) {
                setPlayerToFinished(player_idx, state);
                break;
            }
            Player next_player_idx = (player_idx + 1) % count; //index of the idle player
            roles[next_player_idx] = CO_ATTACKER; //swap the values
            roles[player_idx] = IDLE; //maybe even use std::swap?
            setPlayerToFinished(player_idx, state);
            break;
        }
        case IDLE : { //this only is called when multiple people finish
            assert(count >= 4 && "there cannot be an idle with less than 4 players");
            setPlayerToFinished(player_idx, state); 
            break;
        }
    }
}

bool onlyOnePlayerLeft(State &state){
    int active_count = 0;
    for(Player i = 0; i < state.player_count; ++i){
        if(state.player_roles[i] != Protocol::PlayerRole::FINISHED) active_count++;
    }
    return active_count == 1;
}

Player findLastPlayer(State &state){
    using namespace Protocol;
    auto& roles = state.player_roles;

    auto it = std::find_if(roles.begin(), roles.end(), 
        [](PlayerRole role){return role != FINISHED;});
    Player idx = -1;

    if(it != roles.end()) idx = std::distance(roles.begin(), it);

    return idx;
}

void removeFinishedPlayers(State &state){
    std::vector<Player> setOfFinishedPlayers = findFinishedPlayers(state); //loop over all 
    
    if(setOfFinishedPlayers.empty()) {
        movePlayerRoles(state); //doesnt erase, so we have to moveplayer roles here
        return;
    }

    for(Player& p : setOfFinishedPlayers){
        eraseFinishedPlayer(p, state); //erases the player but also moves the player roles to the according positions
    }
    movePlayerRoles(state);

    if(onlyOnePlayerLeft(state)){
        Player durak = findLastPlayer(state);
        state.durak = durak;
    }
}

bool topSlotsClear(State &state){
    for(uint slot = 6; slot < 12; ++slot){
        if(state.middle_cards[slot].has_value()) return false;
    }
    return true;
}

bool nextPlayerHasEnoughCards(Player next_player, State &state){
    using namespace Protocol;
    auto& middle = state.middle_cards;
    auto& hands = state.player_hands;

    uint cards_in_middle = 0;
    for(auto& slot : middle) if(slot.has_value()) cards_in_middle++;
    if(hands[next_player].size() > cards_in_middle) return true;

    return false;
}

bool ranksMatchToPassOn(Protocol::Rank rank, State &state){
    using namespace Protocol;
    auto& middle = state.middle_cards;

    //only loop over the bottom slots
    for(uint slot = 0; slot < 6; ++slot){
        if(!middle[slot].has_value()) continue;
        if(middle[slot].value().rank != rank) return false;
    }
    return true;
}

std::optional<Card> getReflectCard(Player player, State &state){
    using namespace Protocol;
    if(state.player_roles[player] != DEFENDER) return std::nullopt;
    if(!topSlotsClear(state)) return std::nullopt;
    if(!nextPlayerHasEnoughCards((player + 1) % state.player_count, state)) return std::nullopt;

    Suit trump = state.trump_card.suit;
    for(Card card : state.player_hands[player]){
        if(card.suit != trump) continue;
        if(!ranksMatchToPassOn(card.rank, state)) continue;
        return card;
    }
    return std::nullopt;
}

bool attackedWithMaxCards(State &state){ //check if defender can even defend the cards
    auto& middle = state.middle_cards;

    Player defender_idx = findDefender(state);
    uint d_card_count = state.player_hands[defender_idx].size();
    switch(state.battle_type){
        case BATTLETYPE_FIRST: { //max cards are 5

        }
    }
}

void tryPickUp(State &state){
    
}        

void deleteOldBattle(State &state){
    auto& btype = state.battle_type;

    clearMiddle(state);

    switch(btype){
        case BATTLETYPE_FIRST : {
            btype = BATTLETYPE_NORMAL; //start normal game
            distributeNewCards(state);
            movePlayerRoles(state);
            break;
        }    
        case BATTLETYPE_NORMAL : {
            distributeNewCards(state);

            if (state.draw_pile.empty()) {
                btype = BATTLETYPE_ENDGAME; //start the endgame
                removeFinishedPlayers(state); //moves the roles automatically
                break;
            }    
            movePlayerRoles(state);
            break;
        }    
        case BATTLETYPE_ENDGAME : {
            removeFinishedPlayers(state); //moves the roles automatically
            break;
        }    
    }    

}    

void startNewBattle(State &state){
    using namespace Protocol;
    if(onlyOnePlayerLeft(state)) return; //dont start a new battle

    switch(state.stage){
        case GAMESTAGE_DEFEND : {
            break;
        }    
        case GAMESTAGE_POST_PICKUP : {
            tryPickUp(state);
            movePlayerRoles(state);
            break;
        }    
    }    
    state.stage = GAMESTAGE_FIRST_ATTACK;
    //attackers and defender no more available actions
    GameHelpers::resetAvailableActions(state);
}    


namespace GameHelpers {
    
        void cardSetup(State &state){
            state.player_hands.resize(state.player_count); //initializes empty hands
        
            fillDeck(state);
            shuffleCards(state);
            distributeCardsBeginOfGame(state);
            determineTrump(state);
        }

        //finds the first attacker based on the lowest trump, then sets all the other roles
        void findFirstAttacker(State &game_state_m){
            using namespace Protocol;
            auto& Pcount = game_state_m.player_count;
            auto& Phand = game_state_m.player_hands;
            auto& Proles = game_state_m.player_roles;

            /*find attacker*/
            auto isTrump = [&game_state_m](const Card& c){
                return c.suit == game_state_m.trump_card.suit;
            };
            Rank lowest = Rank::RANK_ACE; //lowest trump card
            int first_attacker = -1;
        
            for(Player i = 0; i < Pcount; ++i){
                auto trump_cards_on_hand = Phand[i] | std::views::filter(isTrump);
        
                for(auto const &card : trump_cards_on_hand){
                    if(card.rank < lowest){
                        lowest = card.rank;
                        first_attacker = i;
                    }
                }
            }
            if(first_attacker == -1){
                first_attacker = rand() % Pcount; //sets a random player as the first attacker
            }
            
            /*build role order*/
                std::vector<PlayerRole> roles_seq; //sequence of the roles
                roles_seq.reserve(Pcount);
                roles_seq.push_back(PlayerRole::ATTACKER);
                roles_seq.push_back(PlayerRole::DEFENDER);
                roles_seq.push_back(PlayerRole::CO_ATTACKER);
                roles_seq.insert(roles_seq.end(), Pcount - 3, PlayerRole::IDLE);
            /*assign roles*/
                for(int i = 0; i < Pcount; ++i){
                    int who = (first_attacker + i) % Pcount;
                    Proles[who] = roles_seq[i];
                }
        }

        //attacker or coattacker can trigger
        void doneEvent(Player player, State &state){
            using namespace Protocol;
            auto& count = state.player_count;
            auto& roles = state.player_roles;
            auto& ok = state.ok_msg;
            auto& avail_act = state.available_actions;
            
            if(roles[player] == ATTACKER) {
                ok[ATTACKER] = true;
                avail_act[player].clear(); //player has no more available actions
            } 
            if(roles[player] == CO_ATTACKER) {
                ok[CO_ATTACKER] = true;
                avail_act[player].clear(); //player has no more available actions
            }

            switch(state.stage){
                case GAMESTAGE_DEFEND : {
                    if(count == 2) ok[CO_ATTACKER] = true; 
                    
                    if(ok[ATTACKER] && ok[CO_ATTACKER]){
                        deleteOldBattle(state);
                        startNewBattle(state);
                    }
                    break;
                }
                case GAMESTAGE_POST_PICKUP : {
                    if(count == 2) ok[CO_ATTACKER] = true;

                    if(ok[ATTACKER] && ok[CO_ATTACKER]){
                        tryPickUp(state);
                        deleteOldBattle(state);
                        startNewBattle(state);
                    }
                    break;
                }
            }
        }

        //only defender can trigger, it should only light up when possible to trigger
        //but we do checks anyways
        void reflectEvent(State &state){
            using namespace Protocol;
            //it only happens in gamestage open and only defender triggers

            //check if defender has the card with the same rank
            Player defender_idx = (findAttacker(state) + 1) % state.player_count;
            auto card = getReflectCard(defender_idx, state);
            if(!card.has_value()) return;

            movePlayerRoles(state);
            state.available_actions[defender_idx].clear();
        }
        
        void pickUpEvent(State &state){
            using namespace Protocol;
            //only defender can trigger this
            //it should only be visible when theres undefended cards
            //i.e. battle stage must be Open
            if(state.stage != GAMESTAGE_OPEN) return;
            //set stage to post pickup
            state.stage = GAMESTAGE_POST_PICKUP;
            //if attackedWithMaxCards -> startnextbattle
            //else wait till the others pressed done
            int defender_idx = (findAttacker(state) + 1) % state.player_count;
            state.available_actions[defender_idx].clear();
        }

        void attackCard(State &state){
            //check if validMoveAttacker 
            //placeCard & change stage
            //check if still cards, if not set to finish
        }
        //useless? probably
        void resetAvailableActions(State &state){
            using namespace Protocol;

            //if card was played and first attack -> defender pick up
            //if card was played and first attack and d has matching rank -> defender pick  & reflect
            //if card was played but not defended -> defender pick up 

            //if cards were all defended and not button pressed -> attacker & coattacker ready
            //if cards all defended and attacker pressed button
        }
}