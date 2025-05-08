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

//doneEvent Helpers and more
// role/hand removal
void movePlayerRoles(State &state);
void movePlayerRolesOneBack(State &state);
std::vector<Player> findFinishedPlayers(State &state);
void setPlayerToFinished(Player player_idx, State &state);
void eraseFinishedPlayer(Player player_idx, State &state);
bool onlyOnePlayerLeft(State &state);
Player findLastPlayer(State &state);
void removeFinishedPlayers(State &state);

// battle flow
void deleteOldBattle(State &state);
void startNewBattle(State &state);

//reflect Event helpers And more
bool topSlotsClear(State &state);
bool nextPlayerHasEnoughCards(Player next_player, State &state);
bool ranksMatchToPassOn(Protocol::Rank rank, State &state);
std::optional<Card> getReflectCard(Player player, State &state);

bool attackedWithMaxCards(State &state);
std::pair<uint, uint> countCardsInMiddle(State &state);