#include <gtest/gtest.h>
#include "../Server/gamehelper.hpp"
#include "../Server/helperheader.hpp"
#include "../Server/gamelogic.hpp"
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

TEST(FillDeck, Has52Cards) {
    State s{4};
    fillDeck(s);
    EXPECT_EQ(s.draw_pile.size(), 52);
}

TEST(Shuffle, CardsAreDifferentOrder) {
    State s{4};
    fillDeck(s);
    auto before = s.draw_pile;
    shuffleCards(s);
    auto after = s.draw_pile;

    EXPECT_NE(before, after);
    EXPECT_EQ(before.size(), after.size());
}

TEST(DistributeCardsBegin, EveryPlayerHas6cards){
    State s{4};
    fillDeck(s);
    shuffleCards(s);
    distributeCardsBeginOfGame(s);

    EXPECT_EQ(6, s.player_hands[0].size());
    EXPECT_EQ(6, s.player_hands[1].size());
    EXPECT_EQ(6, s.player_hands[2].size());
    EXPECT_EQ(6, s.player_hands[3].size());
}

TEST(DetermineTrump, CheckThatATrumpWasGiven){
    State s{4};
    fillDeck(s);
    shuffleCards(s);
    distributeCardsBeginOfGame(s);
    auto trump_suit = SUIT_count;
    auto trump_rank = RANK_count;
    determineTrump(s);
    trump_suit = s.trump_card.suit;
    trump_rank = s.trump_card.rank;

    EXPECT_NE(SUIT_count, trump_suit);
    EXPECT_NE(RANK_count, trump_rank);

}

TEST(PlayerRoles, MoveTheRoles){
    State s{4};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);
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
    State s{3};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after first removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);
    s.player_hands[new_attacker_idx].clear();

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after second removal" << std::endl;
    printRoles_TEST_HELPER(s);


    EXPECT_EQ(1, s.player_count);
}

TEST(RemoveFinishedPlayer, RemoveAttacker3p){
    State s{3};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);


    EXPECT_EQ(2, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[new_attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker4p){
    State s{4};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);


    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[new_attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker5p){
    State s{5};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);


    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[new_attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveAttacker6p){
    State s{6};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int attacker_idx = findAttacker_TESTHELPER(s);
    
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    
    s.player_hands[attacker_idx].clear(); //clear attackers hand
    
    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_attacker_idx = findAttacker_TESTHELPER(s);


    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(ATTACKER, s.player_roles[new_attacker_idx]);
}

TEST(RemoveFinishedPlayer, RemoveDefender2p){
    State s{3};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    //more rigorous check
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < defender_idx){
        check_flag = true;
    }

    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after first removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;
    s.player_hands[new_defender_idx].clear();

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after second removal" << std::endl;
    printRoles_TEST_HELPER(s);

    EXPECT_EQ(1, s.player_count);
}

TEST(RemoveFinishedPlayer, RemoveDefender3p){
    State s{3};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    //more rigorous check
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < defender_idx){
        check_flag = true;
    }

    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    EXPECT_EQ(2, s.player_count);
    EXPECT_EQ(DEFENDER, s.player_roles[new_defender_idx]); //defender exists
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], DEFENDER);
}

TEST(RemoveFinishedPlayer, RemoveDefender4p){
    State s{4};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    //more rigorous check
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < defender_idx){
        check_flag = true;
    }

    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(DEFENDER, s.player_roles[new_defender_idx]);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], CO_ATTACKER);
}

TEST(RemoveFinishedPlayer, RemoveDefender5p){
    State s{5};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    //more rigorous check
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < defender_idx){
        check_flag = true;
    }

    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    EXPECT_EQ(4, s.player_count);
    EXPECT_EQ(DEFENDER, s.player_roles[new_defender_idx]);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
}

TEST(RemoveFinishedPlayer, RemoveDefender6p){
    State s{6};
    GameHelpers::cardSetup(s);

    GameHelpers::findFirstAttacker(s);

    s.draw_pile.clear(); //clear deck
    
    //find attacker
    int defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);

    s.player_hands[defender_idx].clear(); //clear attackers hand

    //more rigorous check
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < defender_idx){
        check_flag = true;
    }

    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_defender_idx = (findAttacker_TESTHELPER(s) + 1) % s.player_count;

    EXPECT_EQ(5, s.player_count);
    EXPECT_EQ(DEFENDER, s.player_roles[new_defender_idx]);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker3p){
    State s{3};
    GameHelpers::cardSetup(s);
    GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int coattack_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    //more rigorous check for correct role assignment
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < coattack_idx){    
        check_flag = true;
    }


    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int coattacker_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    EXPECT_EQ(2, s.player_count);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], DEFENDER);
}

TEST(RemoveFinishedPlayer, RemoveCoAttacker4p){
    State s{4};
    GameHelpers::cardSetup(s);
    GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int coattack_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    //more rigorous check for correct role assignment
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < coattack_idx){    
        check_flag = true;
    }


    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int new_coattack_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;

    EXPECT_EQ(3, s.player_count);
    EXPECT_EQ(CO_ATTACKER, s.player_roles[new_coattack_idx]);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], CO_ATTACKER);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker5p){
    State s{5};
    GameHelpers::cardSetup(s);
    GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int coattack_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    //more rigorous check for correct role assignment
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < coattack_idx){    
        check_flag = true;
    }


    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int coattacker_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    EXPECT_EQ(4, s.player_count);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
}

TEST(RemoveFinishedPlayer, RemoveCoattacker6p){
    State s{6};
    GameHelpers::cardSetup(s);
    GameHelpers::findFirstAttacker(s);
    s.draw_pile.clear(); //clear deck
    
    //find coattacker
    int coattack_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    //setup the player hands in a controlled manner
    setupHandsOfPlayers_TESTHELPER(s);
    s.player_hands[coattack_idx].clear(); //clear coattackers hand

    //more rigorous check for correct role assignment
    int attacker_idx = findAttacker_TESTHELPER(s);
    bool check_flag = false;
    if(attacker_idx < coattack_idx){    
        check_flag = true;
    }


    std::cout << "roles before removal" << std::endl;
    printRoles_TEST_HELPER(s);

    removeFinishedPlayers(s); //should remove finished player and setup new roles

    std::cout << "roles after removal" << std::endl;
    printRoles_TEST_HELPER(s);

    int coattacker_idx = (findAttacker_TESTHELPER(s) + 2) % s.player_count;
    EXPECT_EQ(5, s.player_count);
    if(check_flag) EXPECT_EQ(s.player_roles[attacker_idx], IDLE);
}