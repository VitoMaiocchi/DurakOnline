#include <gtest/gtest.h>
#include "../Server/include/game.hpp"
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>
#include "../include/server.hpp"

// Define static members
std::set<unsigned int> DurakServer::clients;
std::map<unsigned int, Player> DurakServer::players_map;


// Test fixture for the Game class
class DurakGameTest : public ::testing::Test {
protected:
    Game* game; // Use pointer to reset easily
    std::set<ClientID> clients;

    void SetUp() override {
        clients = {1, 2, 3}; // Initialize clients
        game = new Game(clients);
    }

    void TearDown() override {
        delete game;
    }
};

// Test: Verify roles are assigned correctly during game construction
TEST_F(DurakGameTest, TestGameConstructor_PlayerRoles) {
    ASSERT_EQ(game->getPlayerRoles().size(), clients.size())
        << "Player roles not initialized correctly";

    for (const auto& [id, role] : game->getPlayerRoles()) {
        EXPECT_TRUE(role == ATTACKER || role == DEFENDER || role == CO_ATTACKER || role == IDLE)
            << "Invalid role assigned to player " << id;
    }
}


// Test: Validate trump card and last card initialization
TEST_F(DurakGameTest, TestGameConstructor_TrumpCardInitialization) {
    std::optional<Suit> trump = std::make_optional<Suit>(game->getCardManager()->getTrump());
    ASSERT_NE(trump, std::nullopt)
        << "Trump card suit was not set correctly";

    Card last_card = game->getCardManager()->getLastCard();
    ASSERT_EQ(last_card.suit, trump)
        << "Last card suit does not match the trump suit";
}

// Test: Validate player hands during initialization
TEST_F(DurakGameTest, TestGameConstructor_PlayerHandsInitialization) {
    for (auto client : clients) {
        auto hand = game->getCardManager()->getPlayerHand(client);

        ASSERT_EQ(hand.size(), 6) << "Player " << client << " does not have 6 cards in their hand";

        for (size_t i = 0; i < hand.size(); ++i) {
            for (size_t j = i + 1; j < hand.size(); ++j) {
                ASSERT_NE(hand[i], hand[j])
                    << "Duplicate cards found in player " << client << "'s hand";
            }
        }
    }
}

// Test: Validate player roles and Battle initialization consistency
TEST_F(DurakGameTest, TestGameConstructor_PlayerRolesAndBattleConsistency) {
    ASSERT_NE(game->getCurrentBattle(), nullptr)
        << "Battle object was not initialized during game construction";

    const auto& player_roles = game->getPlayerRoles();
    for (const auto& [id, role] : player_roles) {
        EXPECT_EQ(game->getCurrentBattle()->getPlayerRole(id), role)
            << "Role mismatch for player " << id << " between Game and Battle";
    }
}

// Test: Validate correctness of defender and second attacker logic
TEST_F(DurakGameTest, TestGameConstructor_DefenderAndSecondAttacker) {
    const auto& player_roles = game->getPlayerRoles();

    ClientID first_attacker = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == ATTACKER) {
            first_attacker = id;
            break;
        }
    }
    ASSERT_NE(first_attacker, -1) << "First attacker was not determined";

    auto attacker_it = std::find(clients.begin(), clients.end(), first_attacker);
    ASSERT_NE(attacker_it, clients.end()) << "First attacker not found in player list";

    auto expected_defender = (std::next(attacker_it) == clients.end()) ? clients.begin() : std::next(attacker_it);

    ClientID actual_defender = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == DEFENDER) {
            actual_defender = id;
            break;
        }
    }
    ASSERT_NE(actual_defender, -1) << "Defender was not determined";
    EXPECT_EQ(actual_defender, *expected_defender) << "Defender role assignment is incorrect";

    auto defender_it = std::find(clients.begin(), clients.end(), actual_defender);
    ASSERT_NE(defender_it, clients.end()) << "Defender not found in player list";

    auto expected_second_attacker = (std::next(defender_it) == clients.end()) ? clients.begin() : std::next(defender_it);

    ClientID actual_second_attacker = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == CO_ATTACKER) {
            actual_second_attacker = id;
            break;
        }
    }
    ASSERT_NE(actual_second_attacker, -1) << "Second attacker was not determined";
    EXPECT_EQ(actual_second_attacker, *expected_second_attacker)
        << "Second attacker role assignment is incorrect";
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
