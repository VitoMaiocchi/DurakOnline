#include "instance.hpp"
#include "game.hpp"
#include "gamelogic.hpp"

using namespace GameLogic;

#define Prole game_state_m.player_roles
#define Phand game_state_m.player_hands
#define Pcount game_state_m.player_count
#define Tcard game_state_m.trump_card

void findFirstAttacker(State &game_state_m){
    auto isTrump = [&game_state_m](const Card& c){
        return c.suit == game_state_m.trump_card.suit;
    };
    Rank lowest = Protocol::Rank::RANK_ACE; //lowest trump card
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
    if(first_attacker != -1){
        Prole[first_attacker] = Protocol::PlayerRole::ATTACKER;
    }

}

//ctor
Game::Game(Player player_count, Instance* parent_instance_m) : game_state_m(player_count){

    
    Prole.reserve(player_count); //preallocate enough space

    findFirstAttacker(game_state_m); //find first attacker

    // needs to give out the roles 


}

void handlePlayerAction(Player player, PlayerAction action) {
    switch(action){
        case PlayerAction::GAMEACTION_PASS_ON : {
            //reflectevent
            break;
        }

        case GameLogic::PlayerAction::GAMEACTION_PICK_UP : {
            //doneevent
            break;
        }

        case GameLogic::PlayerAction::GAMEACTION_READY : {
            //pickupevent
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