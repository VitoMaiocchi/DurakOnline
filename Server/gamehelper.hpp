#include "gamelogic.hpp"
#include <random>
#include <algorithm>
#include <cassert>

using namespace GameLogic;

namespace GameHelpers {

void cardSetup(State &state); //sets up the cards at the beginning of the game

//natürlich da nur die functions deklariere wo vo game called werded nöd di lowlevel functions 
//wo nur in gamelogic.cpp sind

//da au alli battle helper functions
void createBattle(State &state);
void movePlayerRoles(State &state); 
// ...

//handlePlayerAction Event helper functions
void doneEvent(Player player, State &state);
void reflectEvent(State &state);
void pickUpEvent(State &state);

void updateAvailableActions(State &state); 
//die werded verschickt und chönd au grad benuzt werde zum luege ob en action wo öpper macht legal isch
}