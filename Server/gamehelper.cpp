#include "gamehelper.hpp"
#include <unordered_map>

void fillDeck(State &state) {
  using namespace Protocol;

  for (Suit suit : {SUIT_CLUBS, SUIT_SPADES, SUIT_DIAMONDS, SUIT_HEARTS}) {
    for (Rank rank : {RANK_TWO, RANK_THREE, RANK_FOUR, RANK_FIVE, RANK_SIX,
                      RANK_SEVEN, RANK_EIGHT, RANK_NINE, RANK_TEN, RANK_JACK,
                      RANK_QUEEN, RANK_KING, RANK_ACE}) {
      state.draw_pile.emplace_back(rank, suit);
    }
  }
}

void shuffleCards(State &state) {
  auto &deck = state.draw_pile;
  // Check if deck has been initialized properly
  assert(deck.size() == 52 &&
         "Deck must contain exactly 52 cards before shuffling");
  // Define pseudo random number generator
  std::random_device rd;
  std::mt19937 g(rd());
  // shuffle the deck
  std::shuffle(deck.begin(), deck.end(), g);
}

void distributeCardsBeginOfGame(State &state) {
  auto &deck = state.draw_pile;
  // distribute cards to player
  for (auto &hand : state.player_hands) {
    // Check if players hands are empty
    assert(hand.empty() && "Player's hand should be empty before dealing");

    // distribute the top 6 cards to the player
    hand.insert(deck.begin(), deck.begin() + 6);

    // remove cards from deck
    deck.erase(deck.begin(), deck.begin() + 6);
  }
}

void determineTrump(State &state) {
  auto &deck = state.draw_pile;
  auto &hands = state.player_hands;

  assert(deck.size() == 52 - (6 * hands.size()) &&
         "Deck must contain exactly 52 cards before determining trump");

  state.trump_card = deck.back();
}

void clearMiddle(State &state) {
  for (auto &opt : state.middle_cards) {
    opt.reset(); // because the slots are std::optional
  }
}

void drawFromMiddle(Player player, State &state) {
  auto &deck = state.draw_pile;
  auto &hands = state.player_hands;

  while (hands[player].size() < 6 && !deck.empty()) {
    hands[player].insert(deck.front());
    deck.pop_front();
  }
}

std::pair<uint, uint>
countCardsInMiddle(State &state) { // first = total, second = undefended
  auto &middle = state.middle_cards;
  const int physical_slots = 6;
  uint total = 0;
  uint undefended = 0;

  for (auto slot = 0; slot < physical_slots;
       ++slot) { // only loop over bottom slots
    if (middle[slot].has_value())
      total++;
    if (middle[slot].has_value() && !middle[slot + 6].has_value())
      undefended++;
  }

  return {total, undefended};
}

//distributes cards after a play
void distributeNewCards(State &state) {
  using namespace Protocol;
  auto &roles = state.player_roles;
  auto &count = state.player_count;

  std::vector<int> drawOrder;
  drawOrder.resize(count);

  /*find the first to draw*/
  int first_idx = 0; // first is currently just the attacker
  auto it = std::find_if(roles.begin(), roles.end(), [](const PlayerRole role) {
    return role == ATTACKER;
  });
  if (it != roles.end()) {
    first_idx = std::distance(roles.begin(), it);
  }

  // fills the  draw order index with numbers, from 0
  std::iota(drawOrder.begin(), drawOrder.end(), 0); 

  // rotates left, so the elements before the first index end up in the back,
  // i.e. 3 4 5 0 1 2 if first_idx was 3
  std::rotate(drawOrder.begin(), drawOrder.begin() + first_idx,
              drawOrder.end());

  // move defender to the end
  int def = drawOrder[1];                 // defender index is
  drawOrder.erase(drawOrder.begin() + 1); // erases the defender index
  drawOrder.push_back(def);               // adds the defender at the end

  // for each player draw six cards
  for (int p : drawOrder)
    drawFromMiddle(p, state);
}

//finds attacking player
Player findAttacker(State &state) {
  using namespace Protocol;
  auto &roles = state.player_roles;

  auto it = std::find_if(roles.begin(), roles.end(),
                         [](PlayerRole role) { return role == ATTACKER; });
  Player attacker_idx = -1;
  if (it != roles.end()) {
    attacker_idx = std::distance(roles.begin(), it);
  }
  return attacker_idx;
}

Player findDefender(State &state) {
  Player defender = (findAttacker(state) + 1) % state.player_count;
  return defender;
}

//rotates player roles counter clock wise
void movePlayerRoles(State &state) {
  using namespace Protocol;
  auto &count = state.player_count;
  auto &roles = state.player_roles;

  // find finished players and save them in the vector
  std::vector<bool> is_finished(count, false); // fixes the finished player
                                               // roles
  for (Player i = 0; i < count; ++i) {
    if (roles[i] == FINISHED) {
      is_finished[i] = true;
    }
  }

  // extract the non finished roles

  std::vector<PlayerRole> movable;
  movable.reserve(count);
  for (Player i = 0; i < count; ++i) {
    if (!is_finished[i])
      movable.push_back(roles[i]);
  }

  // rotate the non finished roles
  std::rotate(movable.begin(), movable.end() - 1, movable.end());

  // put them back into the normal player_roles vector
  for (Player i = 0, j = 0; i < count; ++i) {
    if (!is_finished[i]) {
      roles[i] = movable[j++]; // increment j after inserting role
    }
  }
}

//moves the player roles clockwise -> this is essential for erasing finished players from the game
//a better solution will be appreciated
void movePlayerRolesOneBack(State &state) {
  using namespace Protocol;
  auto &count = state.player_count;
  auto &roles = state.player_roles;

  // find finished players and save them in the vector
  std::vector<bool> is_finished(count, false); // fixes the finished player
                                               // roles
  for (Player i = 0; i < count; ++i) {
    if (roles[i] == FINISHED) {
      is_finished[i] = true;
    }
  }

  // extract the non finished roles

  std::vector<PlayerRole> movable;
  movable.reserve(count);
  for (Player i = 0; i < count; ++i) {
    if (!is_finished[i])
      movable.push_back(roles[i]);
  }

  // rotate to the left
  std::rotate(movable.begin(), movable.begin() + 1, movable.end());

  // put them back into the normal player_roles vector
  for (Player i = 0, j = 0; i < count; ++i) {
    if (!is_finished[i]) {
      roles[i] = movable[j++]; // increment j after inserting role
    }
  }
}

//returns vector of indices of the finished players
//better solution map with key = name_of_player and value being id? idk the current approach works fine
std::vector<Player> findFinishedPlayers(State &state) {
  auto &count = state.player_count;
  auto &hands = state.player_hands;
  auto &roles = state.player_roles;

  assert(count == hands.size() && count == roles.size() &&
         "The sizes and the count must match");

  std::vector<Player> finished_players;
  for (std::size_t i = 0; i < count; ++i) {
    if (hands[i].empty()) {
      finished_players.push_back(static_cast<Player>(i));
    }
  }
  std::sort(finished_players.rbegin(),
            finished_players.rend()); // descending order for easy erasing
  return finished_players;
}

void setPlayerToFinished(Player player_idx, State &state) {
  state.player_roles[player_idx] = Protocol::PlayerRole::FINISHED;
}

//erases finished players from the game -> they can still observe but won't be participants of the battles anymore
void eraseFinishedPlayer(Player player_idx, State &state) {
  using namespace Protocol;
  auto &count = state.player_count;
  auto &roles = state.player_roles;

  switch (roles[player_idx]) {
  case ATTACKER: {
    movePlayerRoles(state); // attacker becomes idle
    setPlayerToFinished(player_idx, state);
    movePlayerRolesOneBack(state);
    break;
  }
  case DEFENDER: {
    movePlayerRoles(state); // defender becomes attacker
    movePlayerRoles(state); // defender becomes idle
    setPlayerToFinished(player_idx, state);
    movePlayerRolesOneBack(state);
    break;
  }
  case CO_ATTACKER: {
    assert(count >= 3 &&
           "there cannot be a coattacker with less than 3 players");
    if (count == 3) {
      setPlayerToFinished(player_idx, state);
      break;
    }
    Player next_player_idx =
        (player_idx + 1) % count;         // index of the idle player
    roles[next_player_idx] = CO_ATTACKER; // swap the values
    roles[player_idx] = IDLE;             // maybe even use std::swap?
    setPlayerToFinished(player_idx, state);
    break;
  }
  case IDLE: { // this only is called when multiple people finish
    assert(count >= 4 && "there cannot be an idle with less than 4 players");
    setPlayerToFinished(player_idx, state);
    break;
  }
  }
}

//a check function
bool onlyOnePlayerLeft(State &state) {
  int active_count = 0;
  for (Player i = 0; i < state.player_count; ++i) {
    if (state.player_roles[i] != Protocol::PlayerRole::FINISHED)
      active_count++;
  }
  return active_count == 1;
}

//function to identify the index of the durak, could potentially be merged with the above function
Player findLastPlayer(State &state) {
  using namespace Protocol;
  auto &roles = state.player_roles;

  auto it = std::find_if(roles.begin(), roles.end(),
                         [](PlayerRole role) { return role != FINISHED; });
  Player idx = -1;

  if (it != roles.end())
    idx = std::distance(roles.begin(), it);

  return idx;
}

//function that loops over all finished players and erases them
void removeFinishedPlayers(State &state) {
  std::vector<Player> setOfFinishedPlayers =
      findFinishedPlayers(state); // loop over all

  if (setOfFinishedPlayers.empty()) {
    movePlayerRoles(state); // doesnt erase, so we have to moveplayer roles here
    return;
  }

  for (Player &p : setOfFinishedPlayers) {
    eraseFinishedPlayer(p, state); // erases the player but also moves the
                                   // player roles to the according positions
  }
  movePlayerRoles(state);

  if (onlyOnePlayerLeft(state)) {
    Player durak = findLastPlayer(state);
    state.durak = durak;
  }
}

//check function -> no card has been defended yet (potential pass on)
bool topSlotsClear(State &state) {
  for (uint slot = 6; slot < 12; ++slot) {
    if (state.middle_cards[slot].has_value())
      return false;
  }
  return true;
}

//check function -> to legitimize a pass on
bool nextPlayerHasEnoughCards(Player next_player, State &state) {
  using namespace Protocol;
  auto &middle = state.middle_cards;
  auto &hands = state.player_hands;

  uint cards_in_middle = 0;
  for (auto &slot : middle)
    if (slot.has_value())
      cards_in_middle++;
  if (hands[next_player].size() > cards_in_middle)
    return true;

  return false;
}

//check function -> potential pass on
bool ranksMatchToPassOn(Protocol::Rank rank, State &state) {
  using namespace Protocol;
  auto &middle = state.middle_cards;

  // only loop over the bottom slots
  for (uint slot = 0; slot < 6; ++slot) {
    if (!middle[slot].has_value())
      continue;
    if (middle[slot].value().rank != rank)
      return false;
  }
  return true;
}

//check function -> to expand attack with more cards
bool atLeastOneCardInMiddleMatchesRank(Protocol::Rank rank, State &state) {
  auto &middle = state.middle_cards;

  for (auto slot : middle) {
    if (slot.has_value() && slot.value().rank == rank)
      return true;
  }
  return false;
}

//finds the trump card in the players hand which can potentially reflect the attack without placing the card
std::optional<Card> getReflectCard(Player player, State &state) {
  using namespace Protocol;
  if (state.player_roles[player] != DEFENDER) //no pass on possib;e
    return std::nullopt;
  if (!topSlotsClear(state)) //no pass on possible
    return std::nullopt;
  if (!nextPlayerHasEnoughCards((player + 1) % state.player_count, state)) //no pass on possible
    return std::nullopt;

  Suit trump = state.trump_card.suit;
  for (Card card : state.player_hands[player]) {
    if (card.suit != trump)
      continue;
    if (!ranksMatchToPassOn(card.rank, state))
      continue;
    return card;
  }
  return std::nullopt;
}

//check function -> max attacks reached?
bool attackedWithMaxCards(
    State &state) { // check if defender can even defend the cards
  auto &middle = state.middle_cards;
  auto total_cards = countCardsInMiddle(state).first;
  auto undefended_cards = countCardsInMiddle(state).second;
  /*TODO*/
  Player defender_idx = findDefender(state);
  auto defender_card_count = state.player_hands[defender_idx].size();

  switch (state.battle_type) {
  case BATTLETYPE_FIRST: { // max cards are 5
    if (total_cards == 5)
      return true; // total cards cannot be > 5
    return false;
  } break;
  case BATTLETYPE_NORMAL: {
    if (undefended_cards == defender_card_count)
      return true; // undefended cards cannot be > cards in hand
    if (total_cards == 6)
      return true; // total cards cannot be > 6
    return false;
  } break;
  case BATTLETYPE_ENDGAME: {
    if (undefended_cards == defender_card_count)
      return true; // undefended cards cannot be > cards in hand
    if (total_cards == 6)
      return true; // total cards cannot be > 6
    // implement a check and return if the last card from a hand has been played
    return false;
  }
  }
  return false;
}

//defender picks up, only happens in certain stages, till now no checks were needed
void tryPickUp(State &state) {
  auto &middle = state.middle_cards;
  auto &hands = state.player_hands;
  Player defender_idx = findDefender(state);
  // do we need checks?
  for (auto slot : middle) {
    if (slot.has_value()) {
      hands[defender_idx].insert(slot.value());
      slot = std::nullopt;
    }
  }
}

//check function -> is it legal to place the card? attacker
bool isValidMoveAttacker(Protocol::Card card, State &state) {
  using namespace Protocol;
  auto &middle = state.middle_cards;

  if (state.ok_msg[ATTACKER])
    return false; // cannot play card after pressing done
  if (attackedWithMaxCards(state))
    return false; // cannot play more cards than possible

  if (state.stage == GAMESTAGE_FIRST_ATTACK) {
    std::cout << "is true becuase its first attack" << std::endl;
    return true;
  }
  // check if the card matches rank with at least one card in the middle
  if (atLeastOneCardInMiddleMatchesRank(card.rank, state)) {
    std::cout << "is true becuase rank matches" << std::endl;
    return true;
  }

  return false;
}

bool isValidMoveCoAttacker(State &state) { 
  /*TODO*/
  return false; 
}
bool isValidMoveDefender(State &state) { 
  /*TODO*/
  return false; 
}

void updateGameStage(State &state){
  using namespace Protocol;
  auto &stage = state.stage;

  uint total = countCardsInMiddle(state).second;
  uint undefended = countCardsInMiddle(state).second;

  switch(stage){
    case GAMESTAGE_FIRST_ATTACK:
      if(total > 0) stage = GAMESTAGE_OPEN;
    break;
    case GAMESTAGE_OPEN:
      if(undefended == 0) stage = GAMESTAGE_DEFEND; //all cards were defended
    break;
    case GAMESTAGE_DEFEND:
      if(undefended > 0) stage = GAMESTAGE_OPEN;
    break;
    case GAMESTAGE_DONE:
    break;
    case GAMESTAGE_POST_PICKUP:
    break;
  }
}
//places card on the field
void placeCard(Player player, Protocol::Card card, State &state,
               Protocol::CardSlot slot = Protocol::CARDSLOT_COUNT) {
  using namespace Protocol;
  auto &hand = state.player_hands;
  auto &middle = state.middle_cards;
  auto &roles = state.player_roles;
  auto &stage = state.stage;
  
  bool placed_a_card = false;

  if (roles[player] == DEFENDER) { // is this check relevant? it should be checked in valid move defender
    if (!middle[slot].has_value()){
      middle[slot] = card;
      placed_a_card = true;
      updateGameStage(state);
    }
    return;
  }
  // attacker or coattacker
  for (uint s = CARDSLOT_1; s < CARDSLOT_1_TOP; ++s) {
    if (!middle[s].has_value()) {
      middle[s] = card;
      placed_a_card = true;
      updateGameStage(state);
      return;
    }
  }
  std::cout << "no free slot available to place card" << std::endl;
}

/*TODO: finished this function*/
//attacker or coattacker want to attack
/* 
attackCard, takes a set of cards, and checks that all cards are valid either on their own if its just one card or all together
it needs to validate all cards
then place all cards in the first free bottom slots
delete from player hand while placing the cards
*/
void attackCard(std::unordered_set<Protocol::Card>& cards, State &state) {
  using namespace Protocol;
  auto &middle = state.middle_cards; //middle slots
  auto &hands  = state.player_hands;
  auto &btype  = state.battle_type;
  auto &stage  = state.stage;

  Player attacker_id = findAttacker(state);
  Player defender_id = findDefender(state);

// ================================================= this part is all checks to protect bad behaviour
  
  Rank common_rank = (*cards.begin()).rank; //rank of the first card to check that all have the same rank
  if(stage == GAMESTAGE_FIRST_ATTACK){
    for(auto &card : cards){
      if(common_rank != card.rank) return;
    }
  }
  for(auto &card : cards){
    if (!isValidMoveAttacker(card, state)) return;
  }

  uint total = countCardsInMiddle(state).first;
  uint undefended = countCardsInMiddle(state).second;
  uint defenders_hand_size = state.player_hands[defender_id].size();
  //fetch defenders cards on hand, they should equal or be more than undefended + cards.size()
  if(btype == BATTLETYPE_FIRST){
    if(total + cards.size() > 5) return;
  }
  if(btype == BATTLETYPE_NORMAL || btype == BATTLETYPE_ENDGAME){
    if(total + cards.size() > 6) return;
    if(undefended + cards.size() > defenders_hand_size) return;
  }

  for(auto &card : cards){
    //find card in attackers hand, if yes then good, if no then return
    auto it = hands[attacker_id].find(card);
    if(it == hands[attacker_id].end()){
      std::cerr << "card not found in attackers hand" << std::endl;
      return;
    }
  }
//========================================================================================
std::cout << "passed all the checks" << std::endl;
//==================================== this part is where the cards are placed on the field and erased from the player hand

  while(!cards.empty()){
    Card current_card = *cards.begin();
    placeCard(attacker_id, current_card, state, CARDSLOT_COUNT); //any card slot, place Card determines slot byitself
    hands[attacker_id].erase(current_card); //why doesnt this erase 
    cards.erase(current_card);
  }
}

void deleteOldBattle(State &state) {
  auto &btype = state.battle_type;

  clearMiddle(state);

  switch (btype) {
  case BATTLETYPE_FIRST: {
    btype = BATTLETYPE_NORMAL; // start normal game
    distributeNewCards(state);
    movePlayerRoles(state);
    break;
  }
  case BATTLETYPE_NORMAL: {
    distributeNewCards(state);

    if (state.draw_pile.empty()) {
      btype = BATTLETYPE_ENDGAME;   // start the endgame
      removeFinishedPlayers(state); // moves the roles automatically
      break;
    }
    movePlayerRoles(state);
    break;
  }
  case BATTLETYPE_ENDGAME: {
    removeFinishedPlayers(state); // moves the roles automatically
    break;
  }
  }
}

void startNewBattle(State &state) {
  using namespace Protocol;
  if (onlyOnePlayerLeft(state))
    return; // dont start a new battle

  switch (state.stage) {
  case GAMESTAGE_DEFEND: {
    break;
  }
  case GAMESTAGE_POST_PICKUP: {
    tryPickUp(state);
    movePlayerRoles(state);
    break;
  }
  }
  state.stage = GAMESTAGE_FIRST_ATTACK;
  // attackers and defender no more available actions
  GameHelpers::resetAvailableActions(state);
}

namespace GameHelpers {

void cardSetup(State &state) {
  state.player_hands.resize(state.player_count); // initializes empty hands

  fillDeck(state);
  shuffleCards(state);
  distributeCardsBeginOfGame(state);
  determineTrump(state);
}

// finds the first attacker based on the lowest trump, then sets all the other
// roles
void findFirstAttacker(State &game_state_m) {
  using namespace Protocol;
  auto &Pcount = game_state_m.player_count;
  auto &Phand = game_state_m.player_hands;
  auto &Proles = game_state_m.player_roles;

  /*find attacker*/
  auto isTrump = [&game_state_m](const Card &c) {
    return c.suit == game_state_m.trump_card.suit;
  };
  Rank lowest = Rank::RANK_ACE; // lowest trump card
  int first_attacker = -1;

  for (Player i = 0; i < Pcount; ++i) {
    auto trump_cards_on_hand = Phand[i] | std::views::filter(isTrump);

    for (auto const &card : trump_cards_on_hand) {
      if (card.rank < lowest) {
        lowest = card.rank;
        first_attacker = i;
      }
    }
  }
  if (first_attacker == -1) {
    first_attacker =
        rand() % Pcount; // sets a random player as the first attacker
  }

  /*build role order*/
  std::vector<PlayerRole> roles_seq; // sequence of the roles
  roles_seq.reserve(Pcount);
  roles_seq.push_back(PlayerRole::ATTACKER);
  roles_seq.push_back(PlayerRole::DEFENDER);
  roles_seq.push_back(PlayerRole::CO_ATTACKER);
  roles_seq.insert(roles_seq.end(), Pcount - 3, PlayerRole::IDLE);
  /*assign roles*/
  for (int i = 0; i < Pcount; ++i) {
    int who = (first_attacker + i) % Pcount;
    Proles[who] = roles_seq[i];
  }
}

// attacker or coattacker can trigger Done button
void doneEvent(Player player, State &state) {
  using namespace Protocol;
  auto &count = state.player_count;
  auto &roles = state.player_roles;
  auto &ok = state.ok_msg;
  auto &avail_act = state.available_actions;

  if (roles[player] == ATTACKER) {
    ok[ATTACKER] = true;
    avail_act[player].clear(); // player has no more available actions
  }
  if (roles[player] == CO_ATTACKER) {
    ok[CO_ATTACKER] = true;
    avail_act[player].clear(); // player has no more available actions
  }

  switch (state.stage) {
  case GAMESTAGE_DEFEND: {
    if (count == 2)
      ok[CO_ATTACKER] = true;

    if (ok[ATTACKER] && ok[CO_ATTACKER]) {
      deleteOldBattle(state);
      startNewBattle(state);
    }
    break;
  }
  case GAMESTAGE_POST_PICKUP: {
    if (count == 2)
      ok[CO_ATTACKER] = true;

    if (ok[ATTACKER] && ok[CO_ATTACKER]) {
      tryPickUp(state);
      deleteOldBattle(state);
      startNewBattle(state);
    }
    break;
  }
  }
}

// only defender can trigger, it should only light up when possible to trigger
// but we do checks anyways
void reflectEvent(State &state) {
  using namespace Protocol;
  // it only happens in gamestage open and only defender triggers

  // check if defender has the card with the same rank
  Player defender_idx = (findAttacker(state) + 1) % state.player_count;
  auto card = getReflectCard(defender_idx, state);
  if (!card.has_value())
    return;

  movePlayerRoles(state);
  state.available_actions[defender_idx].clear();
}

void pickUpEvent(State &state) {
  using namespace Protocol;
  // only defender can trigger this
  // it should only be visible when theres undefended cards
  // i.e. battle stage must be Open
  if (state.stage != GAMESTAGE_OPEN)
    return;
  // set stage to post pickup
  state.stage = GAMESTAGE_POST_PICKUP;
  // if attackedWithMaxCards -> startnextbattle
  if (attackedWithMaxCards(state))
    startNewBattle(state);
  // else wait till the others pressed done
  int defender_idx = (findAttacker(state) + 1) % state.player_count;
  state.available_actions[defender_idx].clear();
}

/*TODO: 
- check attacker case
- defender case
- coattacker case
*/
void cardEvent(Player player, std::unordered_set<Protocol::Card> cards,
               State &state) {
  using namespace Protocol;
  auto &roles = state.player_roles;
  size_t amount = cards.size(); // amount of cards played at once
  switch (roles[player]) {
  case ATTACKER: {

    attackCard(cards, state);

    break;
  }
  case DEFENDER:
    break;
  case CO_ATTACKER:
    break;
  default:
    std::cout << "No valid role wants to play a card" << std::endl;
    break;
  }
}
// useless? probably
void resetAvailableActions(State &state) {
  using namespace Protocol;

  // if card was played and first attack -> defender pick up
  // if card was played and first attack and d has matching rank -> defender
  // pick  & reflect if card was played but not defended -> defender pick up

  // if cards were all defended and not button pressed -> attacker & coattacker
  // ready if cards all defended and attacker pressed button
}
} // namespace GameHelpers