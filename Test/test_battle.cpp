#include <gtest/gtest.h>
#include "../Server/include/server.hpp"
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <set>
#include <map>
#include <string>

// Initialize static members of DurakServer
std::set<unsigned int> DurakServer::clients;
std::map<unsigned int, Player> DurakServer::players_map;

// Test case: DurakServer static member initialization
TEST(DurakServerTest, StaticMembersInitialization) {
    EXPECT_TRUE(DurakServer::clients.empty());
    EXPECT_TRUE(DurakServer::players_map.empty());
}

// Test case: Adding a client to DurakServer
TEST(DurakServerTest, AddClient) {
    unsigned int client_id = 1;
    DurakServer::clients.insert(client_id);

    EXPECT_EQ(DurakServer::clients.size(), 1);
    EXPECT_NE(DurakServer::clients.find(client_id), DurakServer::clients.end());
}

// Test case: Initialize player roles in Battle
TEST(BattleTest, InitializePlayerRoles) {
    std::map<unsigned int, PlayerRole> players = {
        {1, PlayerRole::ATTACKER},
        {2, PlayerRole::DEFENDER},
        {3, PlayerRole::IDLE}
    };
    std::set<unsigned int> clients = {1, 2, 3};
    CardManager card_manager(clients);
    
    Battle battle(BattleType::BATTLETYPE_NORMAL, players, card_manager, clients);

    EXPECT_EQ(battle.getPlayerRole(1), PlayerRole::ATTACKER);
    EXPECT_EQ(battle.getPlayerRole(2), PlayerRole::DEFENDER);
    EXPECT_EQ(battle.getPlayerRole(3), PlayerRole::IDLE);
}

// Test case: Sending battle state updates
TEST(BattleTest, SendBattleStateUpdate) {
    std::map<unsigned int, PlayerRole> players = {
        {1, PlayerRole::ATTACKER},
        {2, PlayerRole::DEFENDER},
        {3, PlayerRole::IDLE}
    };
    std::set<unsigned int> clients = {1, 2, 3};
    CardManager card_manager(clients);

    Battle battle(BattleType::BATTLETYPE_NORMAL, players, card_manager, clients);
    DurakServer::clients.insert(1);
    DurakServer::clients.insert(2);
    DurakServer::clients.insert(3);

    EXPECT_NO_THROW(battle.sendBattleStateUpdate());
}

// Test case: Broadcasting a popup message
// TEST(BattleTest, BroadcastPopup) {
//     std::map<unsigned int, PlayerRole> players = {
//         {1, PlayerRole::ATTACKER},
//         {2, PlayerRole::DEFENDER},
//         {3, PlayerRole::IDLE}
//     };
//     std::set<unsigned int> clients = {1, 2, 3};
//     CardManager card_manager(clients);

//     Battle battle(BattleType::BATTLETYPE_NORMAL, players, card_manager, clients);
//     DurakServer::clients.insert(1);
//     DurakServer::clients.insert(2);
//     DurakServer::clients.insert(3);

//     std::string popup_message = "Game Starting!";
//     EXPECT_NO_THROW(battle.broadcastPopup(popup_message));
// }

// Test case: Card manager update
TEST(CardManagerTest, CardUpdate) {
    std::set<unsigned int> players = {1, 2, 3};
    CardManager card_manager(players);

    EXPECT_NO_THROW(card_manager.cardUpdate());
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
