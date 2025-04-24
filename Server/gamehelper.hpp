#include "gamelogic.hpp"
#include <random>
#include <algorithm>
#include <cassert>
#include <ranges>

using namespace GameLogic;

namespace GameHelpers {

void cardSetup(State &state); //sets up the cards at the beginning of the game

void findFirstAttacker(State &game_state_m); //finds first attacker

//handlePlayerAction Event helper functions
void doneEvent(Player player, State &state);
void reflectEvent(State &state);
void pickUpEvent(State &state);

void updateAvailableActions(State &state); 
//die werded verschickt und chönd au grad benuzt werde zum luege ob en action wo öpper macht legal isch
}