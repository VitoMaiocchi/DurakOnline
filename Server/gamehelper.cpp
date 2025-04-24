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
    // Check if deck has been initialized properly
    assert(state.draw_pile.size() == 52 && "Deck must contain exactly 52 cards before shuffling");
    // Define pseudo random number generator
    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle the deck
    std::shuffle(state.draw_pile.begin(), state.draw_pile.end(), g);
}
    
void distributeCardsBeginOfGame(State &state){
    // distribute cards to player
    for (auto& hand : state.player_hands){
        // Check if players hands are empty
        assert(hand.empty() && "Player's hand should be empty before dealing");
        
        // distribute the top 6 cards to the player
        hand.insert(state.draw_pile.begin(), state.draw_pile.begin()+6);
        
        // remove cards from deck
        state.draw_pile.erase(state.draw_pile.begin(), state.draw_pile.begin()+6);
    }
}
    
void determineTrump(State &state){
    assert(state.draw_pile.size() == 52 - (6 * state.player_hands.size()) && "Deck must contain exactly 52 cards before determining trump");
    
    state.trump_card = state.draw_pile.back();
}   

void clearMiddle(State &state){
    for(auto &opt : state.middle_cards){
        opt.reset();
    }
}

void drawFromMiddle(Player player, State &state){
    while(state.player_hands[player].size() < 6 && !state.draw_pile.empty()){ 
        state.player_hands[player].insert(state.draw_pile.front());
        state.draw_pile.pop_front();
    }
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

void movePlayerRoles(State &state){
    //rotate to the right. 
    // e.g. [A, D, C, I, I] → [I, A, D, C, I]
    std::rotate(state.player_roles.begin(), state.player_roles.end() - 1, state.player_roles.end());
}

std::vector<Player> findFinishedPlayers(State &state){
    assert(state.player_count == state.player_hands.size() && state.player_count == state.player_roles.size() && "The sizes and the count must match");
    std::vector<Player> finished_players;
    for(std::size_t i = 0; i < state.player_count; ++i){
        if(state.player_hands[i].empty()){
            finished_players.push_back(static_cast<Player>(i));
        }
    }
    std::sort(finished_players.rbegin(), finished_players.rend()); //descending order for easy erasing
    return finished_players;
}

void eraseFromRolesAndHands(Player player_idx, State &state){
    auto it_roles = state.player_roles.begin() + player_idx;
    auto it_hands = state.player_hands.begin() + player_idx;

    state.player_roles.erase(it_roles);
    state.player_hands.erase(it_hands);
    state.player_count--;
}

void eraseFinishedPlayer(Player player_idx, State &state){
    using namespace Protocol;
    switch(state.player_roles[player_idx]){
        case ATTACKER : {
            movePlayerRoles(state); //attacker becomes idle
            eraseFromRolesAndHands(player_idx, state);
            break;
        }
        case DEFENDER : {
            movePlayerRoles(state); //defender becomes attacker
            movePlayerRoles(state); //defender becomes idle
            eraseFromRolesAndHands(player_idx, state);
            break;
        }
        case CO_ATTACKER : {
            assert(state.player_count >= 3 && "there cannot be a coattacker with less than 3 players");
            if(state.player_count == 3) {
                eraseFromRolesAndHands(player_idx, state);
                movePlayerRoles(state); //update turn order for next battle
                break;
            }
            Player next_player_idx = (player_idx + 1) % state.player_count; //index of the idle player
            state.player_roles[next_player_idx] = CO_ATTACKER; //swap the values
            state.player_roles[player_idx] = IDLE; //maybe even use std::swap?
            eraseFromRolesAndHands(player_idx, state);
            movePlayerRoles(state);
            break;
        }
        case IDLE : {
            assert(state.player_count >= 4 && "there cannot be an idle with less than 4 players");
            eraseFromRolesAndHands(player_idx, state); 
            movePlayerRoles(state); //update turn order for next battle
            break;
        }
    }
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
    
}


void deleteOldBattle(State &state){
    clearMiddle(state);

    switch(state.battle_type){
        case BATTLETYPE_FIRST : {
            state.battle_type = BATTLETYPE_NORMAL; //start normal game
            distributeNewCards(state);
            movePlayerRoles(state);
            break;
        }
        case BATTLETYPE_NORMAL : {
            distributeNewCards(state);

            if (state.draw_pile.empty()) {
                state.battle_type = BATTLETYPE_ENDGAME; //start the endgame
                removeFinishedPlayers(state);
                break;
            }
            movePlayerRoles(state);
            break;
        }
        case BATTLETYPE_ENDGAME : {
            removeFinishedPlayers(state);
            break;
        }
    }

}

void startNewBattle(State &state){
    using namespace Protocol;
    switch(state.stage){
        case GAMESTAGE_DEFEND : {
            break;
        }
        case GAMESTAGE_POST_PICKUP : {
            movePlayerRoles(state);
            break;
        }
    }
    state.stage = Protocol::GameStage::GAMESTAGE_FIRST_ATTACK;
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
            
            if(state.player_roles[player] == ATTACKER) state.ok_msg[ATTACKER] = true; 
            if(state.player_roles[player] == CO_ATTACKER) state.ok_msg[CO_ATTACKER] = true;

            switch(state.stage){
                case GAMESTAGE_DEFEND : {
                    if(state.player_count == 2) state.ok_msg[CO_ATTACKER] = true; 
                    
                    if(state.ok_msg[ATTACKER] && state.ok_msg[CO_ATTACKER]){
                        deleteOldBattle(state);
                        startNewBattle(state);
                    }
                    break;
                }
                case GAMESTAGE_POST_PICKUP : {
                    if(state.player_count == 2) state.ok_msg[CO_ATTACKER] = true;

                    if(state.ok_msg[ATTACKER] && state.ok_msg[CO_ATTACKER]){
                        deleteOldBattle(state);
                        startNewBattle(state);
                    }
                    break;
                }
            }
            
        }

        //only defender can trigger
        void reflectEvent(State &state){/*TODO*/}
        void pickUpEvent(State &state){/*TODO*/}

        void updateAvailableActions(State &state){
            switch(state.stage){
                case Protocol::GameStage::GAMESTAGE_FIRST_ATTACK : {
                    /*no available actions*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_OPEN : {
                    /*pick up and/or reflect for defender*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_DEFEND : {
                    /*ready for both attacker*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_POST_PICKUP : {
                    /*ready for both attackers*/
                    break;
                }
                case Protocol::GameStage::GAMESTAGE_DONE : {
                    /*no available actions, destroy battle, setup new battle*/
                    break;
                }
            }
        }
}