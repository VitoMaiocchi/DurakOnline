#include "gamelogic.hpp"
#include <random>
#include <algorithm>
#include <cassert>
#include <ranges>

using namespace GameLogic;

// deck setup
void fillDeck(State &state);
void shuffleCards(State &state);
void distributeCardsBeginOfGame(State &state);
void determineTrump(State &state);

// middle-pile helpers
void clearMiddle(State &state);
void drawFromMiddle(Player player, State &state);
void distributeNewCards(State &state);

// role/hand removal
std::vector<Player> findFinishedPlayers(State &state);
void movePlayerRoles(State &state);
void eraseFromRolesAndHands(Player player_idx, State &state);
void eraseFinishedPlayer(Player player_idx, State &state);
void removeFinishedPlayers(State &state);

// battle flow
void deleteOldBattle(State &state);
void startNewBattle(State &state);
