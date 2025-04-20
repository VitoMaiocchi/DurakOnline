#include "game.hpp"

//ctor
Game::Game(Player player_count, Instance* parent_instance_m){

    player_roles_m.reserve(player_count); //preallocate enough space

    // needs to create a cardmanager, which will be alive for the whole game

    // has to find first attacker -> either lowest trump or next player in order from prev durak

    // needs to give out the roles 

}

