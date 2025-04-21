#include "instance.hpp"
#include "game.hpp"
#include "gamelogic.hpp"

using namespace GameLogic;

#define pr game_state_m.player_roles

void findFirstAttacker(State &game_state_m){
    
}

//ctor
Game::Game(Player player_count, Instance* parent_instance_m) {

    
    pr.reserve(player_count); //preallocate enough space

    findFirstAttacker(game_state_m); //find first attacker

    // needs to give out the roles 


}

void handlePlayerAction(Player player, PlayerAction action) {
    switch(action){
        case PlayerAction::GAMEACTION_PASS_ON : {
            
            break;
        }

        case GameLogic::PlayerAction::GAMEACTION_PICK_UP : {

            break;
        }

        case GameLogic::PlayerAction::GAMEACTION_READY : {

            break;
        }
    }
    
    // helper helper(staet)
    // helper(state)

    // computeAvailableActions()

    // broadcastState()
    // senndStaet()
}

void playerCardNotify(GameLogic::Player player, uint slot, std::list<GameLogic::Card> card) {
    // helper helper(staet)
    // helper(state)

    // computeAvailableActions()

    // broadcastState()
    // senndStaet()
}