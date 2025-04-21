#include "instance.hpp"
#include "game.hpp"
#include "card_manager.hpp"

//ctor
Game::Game(Player player_count, Instance* parent_instance_m) {

    card_manager_m = new CardManager(player_count); //create a new cardmanager for the game

    player_roles_m.reserve(player_count); //preallocate enough space

    findFirstAttacker(); //find first attacker

    // needs to give out the roles 

}

void Game::findFirstAttacker(){

}