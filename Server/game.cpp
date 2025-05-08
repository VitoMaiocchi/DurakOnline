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
#define GStage game_state_m.stage


//ctor
Game::Game(Player player_count, Instance* parent_instance_m, Player previous_durak) {
    
    game_state_m.player_count = player_count;
    game_state_m.middle_cards = {}; 
    game_state_m.durak = previous_durak;
    game_state_m.player_roles.resize(player_count);
    game_state_m.player_hands.resize(player_count);
    game_state_m.available_actions.resize(player_count);
    game_state_m.ok_msg = {{Protocol::PlayerRole::ATTACKER, false}, {Protocol::PlayerRole::CO_ATTACKER, false}};

    GameHelpers::cardSetup(game_state_m); //setup deck, distribute cards, etc..

    GameHelpers::findFirstAttacker(game_state_m); //find first attacker
    
    //start first battle
    Btype = BattleType::BATTLETYPE_FIRST;
    GStage = Protocol::GameStage::GAMESTAGE_FIRST_ATTACK;

}

void Game::handlePlayerAction(Player player, PlayerAction action) {
    switch(action){
        case PlayerAction::GAMEACTION_REFLECT : {
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
            GameHelpers::doneEvent(player, game_state_m);
            //sendprivateState
            //broadcastState
            break;
        }
        default :
            /*do nothing*/
            break;
    }
    
    // helper helper(staet)
    // helper(state)

    // GameHelpers::updateAvailableActions(game_state_m);

    // broadcastState()
    // senndStaet()
}

void Game::playerCardEvent(GameLogic::Player player, uint slot, std::list<GameLogic::Card> card) {
    switch(Proles[player]){
        case Protocol::PlayerRole::ATTACKER : {
            //attackerCardEvent
            break;
        }
        case Protocol::PlayerRole::DEFENDER : {
            //defenderCardEvent
            break;
        }
        case Protocol::PlayerRole::CO_ATTACKER : {
            //coattackerCardEvent
            break;
        }
        default :
            break;
    }
    
    
    // helper helper(staet)
    // helper(state)

    // computeAvailableActions()

    // broadcastState()
    // senndStaet()
}