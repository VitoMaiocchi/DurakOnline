#include "instance.hpp"
#include "game.hpp"
#include "card_manager.hpp"

//ctor
Game::Game(GameLogic::Player player_count, Instance* parent_instance_m) : card_manager_m(player_count){

    player_roles_m.reserve(player_count); //preallocate enough space

    findFirstAttacker(); //find first attacker

    // needs to give out the roles 

}

void Game::findFirstAttacker(){
    
}