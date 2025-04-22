#include "gamelogic.hpp"
#include <random>
#include <algorithm>
#include <cassert>

using namespace GameLogic;

namespace GameHelpers {

//die münd nöd i dem file si. es chan den zb eifach en battle helper und card helper file ge.
void cardSetup(State &state);
void determineTrump(State &state);
void shuffleCards(State &state);
void fillDeck(State &state);

void distributeCardsBeginOfGame(State &state);
void distributeNewCards(State &state);

//natürlich da nur die functions deklariere wo vo game called werded nöd di lowlevel functions 
//wo nur in gamelogic.cpp sind

//da au alli battle helper functions
void movePlayerRoles(State &state); 
// ...

void updateAvailableActions(State &state); 
//die werded verschickt und chönd au grad benuzt werde zum luege ob en action wo öpper macht legal isch
}