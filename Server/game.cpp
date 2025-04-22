#include "instance.hpp"
#include "game.hpp"
#include "gamelogic.hpp"

using namespace GameLogic;

//P is for Player
//T is for Trump
//B is for Battle
#define Proles game_state_m.player_roles
#define Phand game_state_m.player_hands
#define Pcount game_state_m.player_count
#define Tcard game_state_m.trump_card
#define Btype game_state_m.battle_type

//finds the first attacker based on the lowest trump, then sets all the other roles
void findFirstAttacker(State &game_state_m){
    using namespace Protocol;
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

//ctor
Game::Game(Player player_count, Instance* parent_instance_m) : game_state_m(player_count){
    
    GameHelpers::cardSetup(game_state_m); //setup deck, distribute cards, etc..

    findFirstAttacker(game_state_m); //find first attacker

    //start first battle
    Btype = BattleType::BATTLETYPE_FIRST;

}

void Game::handlePlayerAction(Player player, PlayerAction action) {
    switch(action){
        case PlayerAction::GAMEACTION_PASS_ON : {
            //reflectevent
            GameHelpers::reflectEvent(game_state_m);
            break;
        }

        case GameLogic::PlayerAction::GAMEACTION_PICK_UP : {
            //pickupevent
            GameHelpers::pickUpEvent(game_state_m);
            break;
        }
        
        case GameLogic::PlayerAction::GAMEACTION_READY : {
            //doneevent
            GameHelpers::doneEvent(game_state_m);
            break;
        }
    }
    
    // helper helper(staet)
    // helper(state)

    GameHelpers::updateAvailableActions(game_state_m);

    // broadcastState()
    // senndStaet()
}

void Game::playerCardNotify(GameLogic::Player player, uint slot, std::list<GameLogic::Card> card) {
    // helper helper(staet)
    // helper(state)

    // computeAvailableActions()

    // broadcastState()
    // senndStaet()
}