#include <gtest/gtest.h>
#include "../Server/gamehelper.hpp"
#include "../Server/helperheader.hpp"
#include "../Server/gamelogic.hpp"
#include "../Server/game.hpp"
#include <tuple>
#include <algorithm>

using namespace GameHelpers;
using namespace Protocol;


static auto card_cmp = [](Card const &a, Card const &b) {
    return std::tie(a.rank, a.suit) < std::tie(b.rank, b.suit);
};


int findAttacker_TESTHELPER(State& s){
    auto attacker_it = std::find_if(s.player_roles.begin(), s.player_roles.end(), 
        [](PlayerRole role){return role == ATTACKER;});
    int attacker_idx = -1;
    if(attacker_it != s.player_roles.end()){
        attacker_idx = std::distance(s.player_roles.begin(), attacker_it);
    }
    return attacker_idx;
}

void setupHandsOfPlayers_TESTHELPER(State &s){
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = -1;
    int idlefirs_idx = -1;
    int idleseco_idx = -1;
    int idlethir_idx = -1;

    //clear all hands
    for(int offset = 0; offset < s.player_count; ++offset){
        s.player_hands[(attacker_idx + offset) % s.player_count].clear();
    }

    //hand out cards, controlled
    s.player_hands[attacker_idx].insert({RANK_TWO, SUIT_CLUBS});
    s.player_hands[defender_idx].insert({RANK_THREE, SUIT_CLUBS});

    switch(s.player_count){
        case 2 : {
            break;
        }
        case 3 : {
            coattack_idx = (defender_idx + 1) % s.player_count;
            s.player_hands[coattack_idx].insert({RANK_TWO, SUIT_HEARTS});
            break;
        }
        case 4 : {
            coattack_idx = (defender_idx + 1) % s.player_count;
            s.player_hands[coattack_idx].insert({RANK_TWO, SUIT_HEARTS});
            idlefirs_idx = (coattack_idx + 1) % s.player_count;
            s.player_hands[idlefirs_idx].insert({RANK_FOUR, SUIT_HEARTS});
            break;
        }
        case 5 : {
            coattack_idx = (defender_idx + 1) % s.player_count;
            s.player_hands[coattack_idx].insert({RANK_TWO, SUIT_HEARTS});
            idlefirs_idx = (coattack_idx + 1) % s.player_count;
            s.player_hands[idlefirs_idx].insert({RANK_FOUR, SUIT_HEARTS});
            idleseco_idx = (idlefirs_idx + 1) % s.player_count;
            s.player_hands[idleseco_idx].insert({RANK_FIVE, SUIT_HEARTS});
            break;
        }
        case 6 : {
            coattack_idx = (defender_idx + 1) % s.player_count;
            s.player_hands[coattack_idx].insert({RANK_TWO, SUIT_HEARTS});
            idlefirs_idx = (coattack_idx + 1) % s.player_count;
            s.player_hands[idlefirs_idx].insert({RANK_FOUR, SUIT_HEARTS});
            idleseco_idx = (idlefirs_idx + 1) % s.player_count;
            s.player_hands[idleseco_idx].insert({RANK_FIVE, SUIT_HEARTS});
            idlethir_idx = (idleseco_idx + 1) % s.player_count;
            s.player_hands[idlethir_idx].insert({RANK_SIX, SUIT_HEARTS});
            break;
        }
        default : { std::cout << "TOO MANY PLAYERS" << std::endl; break;}
    }

}

void printRoles_TEST_HELPER(State &s){
    for(int i = 0; i < s.player_count; ++i){
        std::cout << "role: " << s.player_roles[i] << std::endl;
    }
}

void placeCardsInMiddleSlot_TESTHELPER(Card card, CardSlot slot, State &s){
    s.middle_cards[slot] = card;
    if(slot < 6) s.stage = GAMESTAGE_OPEN;

    auto cards = countCardsInMiddle(s);
    if(slot >= 6 && cards.second == 0) s.stage = GAMESTAGE_DEFEND;
}



TEST(DistributeCardsBegin, EveryPlayerHas6cards){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // fillDeck(s);
    // shuffleCards(s);
    // distributeCardsBeginOfGame(s);

    EXPECT_EQ(6, s.player_hands[0].size());
    EXPECT_EQ(6, s.player_hands[1].size());
    EXPECT_EQ(6, s.player_hands[2].size());
    EXPECT_EQ(6, s.player_hands[3].size());
}

TEST(DetermineTrump, CheckThatATrumpWasGiven){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // fillDeck(s);
    // shuffleCards(s);
    // distributeCardsBeginOfGame(s);
    auto trump_suit = SUIT_count;
    auto trump_rank = RANK_count;
    determineTrump(s);
    trump_suit = s.trump_card.suit;
    trump_rank = s.trump_card.rank;

    EXPECT_NE(SUIT_count, trump_suit);
    EXPECT_NE(RANK_count, trump_rank);

}

TEST(PlayerRoles, MoveTheRoles){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);
    int attacker_idx = findAttacker_TESTHELPER(s);

    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int idle_idx = (coattacker_idx + 1) % s.player_count;

    movePlayerRoles(s);

    EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
    EXPECT_EQ(s.player_roles[defender_idx], ATTACKER);
    EXPECT_EQ(s.player_roles[coattacker_idx], DEFENDER);
    EXPECT_EQ(s.player_roles[idle_idx], CO_ATTACKER);
}

//remove finished players one by one
TEST(RemoveFinishedPlayer, RemoveAttacker2p){
    Game game(3, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    // std::cout << "roles after first removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);
    s.player_hands[new_attacker_idx].clear();

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    // std::cout << "roles after second removal" << std::endl;
    // printRoles_TEST_HELPER(s);


    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(new_attacker_idx, defender_idx);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[new_attacker_idx]);
    EXPECT_EQ(s.durak, coattack_idx);
}

TEST(RemoveFinishedPlayer, RemoveAttacker3p){
    Game game(3, nullptr, -1);
    State& s = game.getState();

    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[coattack_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;
    
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[coattack_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker5p){
    Game game(5, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[coattack_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(6, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[coattack_idx]);
}

TEST(RemoveFinishedPlayer, RemoveDefender2p){
    Game game(3, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    // std::cout << "roles after first removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;
    s.player_hands[new_defender_idx].clear();

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    // std::cout << "roles after second removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[new_defender_idx]);
    EXPECT_EQ(new_defender_idx, attacker_idx);
    EXPECT_EQ(s.durak, coattacker_idx);
}

TEST(RemoveFinishedPlayer, RemoveDefender3p){
    Game game(3, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[coattack_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveDefender4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[coattack_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveDefender5p){
    Game game(5, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[coattack_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(IDLE, s.player_roles[attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveDefender6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);

    // GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(6, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[coattack_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(IDLE, s.player_roles[attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker3p){
    Game game(3, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[coattack_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(DEFENDER, s.player_roles[attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveCoAttacker4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[coattack_idx]);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker5p){
    Game game(5, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
    EXPECT_EQ(FINISHED, s.player_roles[coattack_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck


    //more rigorous check for correct role assignment
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattack_idx = (defender_idx + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(6, s.player_count);
    EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
    EXPECT_EQ(FINISHED, s.player_roles[coattack_idx]);
}

//remove multiple players at once
TEST(RemoveFinishedPlayer, RemoveAttackerAndDefender4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    
    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();

    //more rigorous check
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[coattacker_idx]);
    // A, D, C, I -> A, D
    // A, D, C, I -> I, A, D, C -> C, I, A, D

}

TEST(RemoveFinishedPlayer, RemoveAttackerAndDefender5p){
    Game game(5, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    //empty the hands
    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[coattacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttackerAndDefender6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(6, s.player_count);
    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[coattacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAtDefCoat4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();
    s.player_hands[coattacker_idx].clear();

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[coattacker_idx]);
    EXPECT_EQ(first_idle, s.durak);
}

TEST(RemoveFinishedPlayer, RemoveAtDefCoat5p){
    Game game(5, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();
    s.player_hands[coattacker_idx].clear();

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[first_idle]);
}

TEST(RemoveFinishedPlayer, RemoveAtDefCoat6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);
    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    s.player_hands[attacker_idx].clear();
    s.player_hands[defender_idx].clear();
    s.player_hands[coattacker_idx].clear();

    // std::cout << "roles before removal" << std::endl;
    // printRoles_TEST_HELPER(s);
    removeFinishedPlayers(s); //should remove finished player and setup new roles
    // std::cout << "roles after removal" << std::endl;
    // printRoles_TEST_HELPER(s);

    EXPECT_EQ(FINISHED, s.player_roles[attacker_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[defender_idx]);
    EXPECT_EQ(FINISHED, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[first_idle]);
}

TEST(ReflectCard, TopSlotsNotClear){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    Card card3 = Card({RANK_THREE, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_2_TOP, s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    reflectEvent(s);
    EXPECT_EQ(false, topSlotsClear(s));
    EXPECT_EQ(DEFENDER, s.player_roles[defender_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[attacker_idx]);
}

TEST(ReflectCard, NoCorrectCardInHand){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    Card card3 = Card({RANK_THREE, SUIT_HEARTS});
    s.player_hands[defender_idx].insert(card3);


    reflectEvent(s);
    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(DEFENDER, s.player_roles[defender_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[attacker_idx]);
}

TEST(ReflectCard, NotEnoughtCards){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    Card card3 = Card({RANK_THREE, SUIT_HEARTS});
    s.player_hands[defender_idx].insert(card3);

    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    reflectEvent(s);
    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(false, nextPlayerHasEnoughCards(coattacker_idx, s));
    EXPECT_EQ(DEFENDER, s.player_roles[defender_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[attacker_idx]);
}

TEST(ReflectCard, RanksDontMatch){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    setupHandsOfPlayers_TESTHELPER(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    Card card3 = Card({RANK_THREE, SUIT_HEARTS});
    s.player_hands[defender_idx].clear();
    s.player_hands[defender_idx].insert(card3);

    Card card4 = Card({RANK_ACE, SUIT_CLUBS});
    Card card5 = Card({RANK_ACE, SUIT_HEARTS});
    s.player_hands[coattacker_idx].insert(card4);
    s.player_hands[coattacker_idx].insert(card5);

    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    reflectEvent(s);
    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(true, nextPlayerHasEnoughCards(coattacker_idx, s));
    EXPECT_EQ(false, ranksMatchToPassOn(card3.rank, s));

    EXPECT_EQ(DEFENDER, s.player_roles[defender_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[attacker_idx]);
}

TEST(ReflectCard, GetCardToReflect){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    setupHandsOfPlayers_TESTHELPER(s);
    Suit trump = s.trump_card.suit;

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    Card card3 = Card({RANK_TWO, trump});
    s.player_hands[defender_idx].clear();
    s.player_hands[defender_idx].insert(card3);

    Card card4 = Card({RANK_ACE, SUIT_CLUBS});
    Card card5 = Card({RANK_ACE, SUIT_HEARTS});
    s.player_hands[coattacker_idx].insert(card4);
    s.player_hands[coattacker_idx].insert(card5);

    auto card_got = getReflectCard(defender_idx, s);

    EXPECT_EQ(card_got.has_value(), true);
    EXPECT_EQ(card_got.value().rank, card3.rank);
    EXPECT_EQ(card_got.value().suit, card3.suit);
}

TEST(ReflectCard, ReflectSuccesfully3p){
    Game game(3, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    Card card3 = Card({RANK_TWO, trump});
    s.player_hands[defender_idx].insert(card3);


    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(true, nextPlayerHasEnoughCards(coattacker_idx,s));
    reflectEvent(s);
    EXPECT_EQ(DEFENDER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[attacker_idx]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
}

TEST(ReflectCard, ReflectSuccesfully4p){
    Game game(4, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    Card card3 = Card({RANK_TWO, trump});
    s.player_hands[defender_idx].insert(card3);


    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(true, nextPlayerHasEnoughCards(coattacker_idx,s));
    reflectEvent(s);
    EXPECT_EQ(DEFENDER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[first_idle]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
}

TEST(ReflectCard, ReflectSuccesfully6p){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    // GameHelpers::cardSetup(s);
    // GameHelpers::findFirstAttacker(s);

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    Card card3 = Card({RANK_TWO, trump});
    s.player_hands[defender_idx].insert(card3);


    EXPECT_EQ(true, topSlotsClear(s));
    EXPECT_EQ(true, nextPlayerHasEnoughCards(coattacker_idx,s));
    reflectEvent(s);
    EXPECT_EQ(DEFENDER, s.player_roles[coattacker_idx]);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[first_idle]);
    EXPECT_EQ(ATTACKER, s.player_roles[defender_idx]);
}

TEST(attackedMaxCards, DidNotAttackWithMaxCards_2a){
    Game game(6, nullptr, -1);
    State& s = game.getState();

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);

    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(false, attackedWithMaxCards(s));
}

//the numbers at the end:
//cards attacked with --- cards defended with --- battle type
TEST(attackedMaxCards, AttackWithMaxCards_5a_0d_0){
    Game game(6, nullptr, -1);
    State& s = game.getState();

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    Card card3 = Card({RANK_TWO, SUIT_SPADES});
    Card card4 = Card({RANK_TWO, SUIT_DIAMONDS});
    Card card5 = Card({RANK_THREE, SUIT_SPADES});
    Card card6 = Card({RANK_THREE, SUIT_DIAMONDS});

    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_3, s);
    placeCardsInMiddleSlot_TESTHELPER(card4, CARDSLOT_4, s);
    placeCardsInMiddleSlot_TESTHELPER(card5, CARDSLOT_5, s);
    // placeCardsInMiddleSlot_TESTHELPER(card6, CARDSLOT_6, s);
    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(true, attackedWithMaxCards(s));
}

TEST(attackedMaxCards, AttackWithMaxCards_5a_2d_0){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    Card card3 = Card({RANK_TWO, SUIT_SPADES});
    Card card4 = Card({RANK_TWO, SUIT_DIAMONDS});
    Card card5 = Card({RANK_THREE, SUIT_SPADES});
    Card card6 = Card({RANK_THREE, SUIT_DIAMONDS});

    Card cardD1 = Card({RANK_THREE, SUIT_CLUBS});
    Card cardD2 = Card({RANK_FOUR, SUIT_HEARTS});

    //attacking cards
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_3, s);
    placeCardsInMiddleSlot_TESTHELPER(card4, CARDSLOT_4, s);
    //def cards
    placeCardsInMiddleSlot_TESTHELPER(cardD1, CARDSLOT_1_TOP, s);
    placeCardsInMiddleSlot_TESTHELPER(cardD2, CARDSLOT_2_TOP, s);

    placeCardsInMiddleSlot_TESTHELPER(card5, CARDSLOT_5, s);
    // placeCardsInMiddleSlot_TESTHELPER(card6, CARDSLOT_6, s);
    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(true, attackedWithMaxCards(s));
}
//the numbers at the end:
//cards attacked with --- cards defended with --- battle type
TEST(attackedMaxCards, AttackWithMaxCards_6a_0d_1){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    s.battle_type = BATTLETYPE_NORMAL;

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    Card card3 = Card({RANK_TWO, SUIT_SPADES});
    Card card4 = Card({RANK_TWO, SUIT_DIAMONDS});
    Card card5 = Card({RANK_THREE, SUIT_SPADES});
    Card card6 = Card({RANK_THREE, SUIT_DIAMONDS});

    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_3, s);
    placeCardsInMiddleSlot_TESTHELPER(card4, CARDSLOT_4, s);
    placeCardsInMiddleSlot_TESTHELPER(card5, CARDSLOT_5, s);
    placeCardsInMiddleSlot_TESTHELPER(card6, CARDSLOT_6, s);
    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(true, attackedWithMaxCards(s));
}

TEST(attackedMaxCards, AttackWithMaxCards_6a_2d_1){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    s.battle_type = BATTLETYPE_NORMAL;

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    Card card3 = Card({RANK_TWO, SUIT_SPADES});
    Card card4 = Card({RANK_TWO, SUIT_DIAMONDS});
    Card card5 = Card({RANK_THREE, SUIT_SPADES});
    Card card6 = Card({RANK_THREE, SUIT_DIAMONDS});

    Card cardD1 = Card({RANK_THREE, SUIT_CLUBS});
    Card cardD2 = Card({RANK_FOUR, SUIT_HEARTS});

    //attacking cards
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_3, s);
    placeCardsInMiddleSlot_TESTHELPER(card4, CARDSLOT_4, s);
    //def cards
    placeCardsInMiddleSlot_TESTHELPER(cardD1, CARDSLOT_1_TOP, s);
    placeCardsInMiddleSlot_TESTHELPER(cardD2, CARDSLOT_2_TOP, s);

    placeCardsInMiddleSlot_TESTHELPER(card5, CARDSLOT_5, s);
    placeCardsInMiddleSlot_TESTHELPER(card6, CARDSLOT_6, s);
    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(true, attackedWithMaxCards(s));
}

TEST(attackedMaxCards, AttackWithMaxCards_3a_2d_1){
    Game game(6, nullptr, -1);
    State& s = game.getState();
    s.battle_type = BATTLETYPE_NORMAL;

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    s.player_hands[defender_idx].clear();
    Card cardD3 = Card({RANK_FIVE, SUIT_DIAMONDS});
    s.player_hands[defender_idx].insert(cardD3);

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    Card card3 = Card({RANK_TWO, SUIT_SPADES});
    Card card4 = Card({RANK_TWO, SUIT_DIAMONDS});
    Card card5 = Card({RANK_THREE, SUIT_SPADES});
    Card card6 = Card({RANK_THREE, SUIT_DIAMONDS});

    Card cardD1 = Card({RANK_THREE, SUIT_CLUBS});
    Card cardD2 = Card({RANK_FOUR, SUIT_HEARTS});

    //attacking cards
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    placeCardsInMiddleSlot_TESTHELPER(card3, CARDSLOT_3, s);
    //def cards
    placeCardsInMiddleSlot_TESTHELPER(cardD1, CARDSLOT_1_TOP, s);
    placeCardsInMiddleSlot_TESTHELPER(cardD2, CARDSLOT_2_TOP, s);

    // std::cout << "small check for stage and btype:\n" << "stage: " << s.stage << " btype: " << s.battle_type<< std::endl;
    EXPECT_EQ(true, attackedWithMaxCards(s));
}

TEST(checkStages, StageFirstAttack){
    using namespace Protocol;
    Game game(6, nullptr, -1);
    State& s = game.getState();

    //stage should be first attack = 0
    EXPECT_EQ(GAMESTAGE_FIRST_ATTACK, s.stage);
}

TEST(checkStages, StageOpen){
    using namespace Protocol;
    Game game(6, nullptr, -1);
    State& s = game.getState();

    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;

    Suit trump = s.trump_card.suit;

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});
    placeCardsInMiddleSlot_TESTHELPER(card1, CARDSLOT_1, s);
    placeCardsInMiddleSlot_TESTHELPER(card2, CARDSLOT_2, s);
    //stage should be first attack = 0
    EXPECT_EQ(GAMESTAGE_OPEN, s.stage);
}


/*TODO:*/
//test available actions 
