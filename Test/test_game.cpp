#include <gtest/gtest.h>
#include "../Server/include/game.hpp"
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>

// Test fixture for the Game class
class DurakGameTest : public ::testing::Test {
protected:
    Game* game; // Use pointer to reset easily
    std::vector<ClientID> clients;

    void SetUp() override {
        clients = {1, 2, 3}; // Initialize clients
        game = new Game(clients);
    }

    void TearDown() override {
        delete game;
    }
};

// Test to verify that roles are assigned correctly during the game constructor
TEST_F(DurakGameTest, TestGameConstructor_PlayerRoles) {
    // Validate the number of player roles matches the number of clients
    ASSERT_EQ(game->getPlayerRoles().size(), clients.size())
        << "Player roles not initialized correctly";

    // Validate that each player has a valid role
    for (const auto& [id, role] : game->getPlayerRoles()) {
        EXPECT_TRUE(role == ATTACKER || role == DEFENDER || role == CO_ATTACKER || role == IDLE)
            << "Invalid role assigned to player " << id;
    }
}

// Test to validate that the Battle object is initialized correctly
TEST_F(DurakGameTest, TestGameConstructor_BattleInitialization) {
    ASSERT_NE(game->getCurrentBattle(), nullptr)
        << "Battle object was not initialized during game construction";

    const auto* first_attacker = game->getCurrentBattle()->getFirstAttackerPtr();
    ASSERT_NE(first_attacker, nullptr)
        << "First attacker pointer is null in the initialized Battle object";
}

// Test to validate trump card and last card initialization
TEST_F(DurakGameTest, TestGameConstructor_TrumpCardInitialization) {
    // Check if the trump card is correctly set
    Suit trump = game->getCardManager()->getTrump();
    ASSERT_NE(trump, SUIT_NONE)
        << "Trump card suit was not set correctly";

    // Check the last card in the deck
    Card last_card = game->getCardManager()->getLastCard();
    ASSERT_EQ(last_card.suit, trump)
        << "Last card suit does not match the trump suit";
}


// Test to validate player hands during initialization
TEST_F(DurakGameTest, TestGameConstructor_PlayerHandsInitialization) {
    for (auto client : clients) {
        auto hand = game->getCardManager()->getPlayerHand(client);

        // Ensure the player has exactly 6 cards
        ASSERT_EQ(hand.size(), 6) << "Player " << client << " does not have 6 cards in their hand";

        // Ensure cards are unique and sorted (optional sorting for validation)
        std::sort(hand.begin(), hand.end(), [&](const Card& a, const Card& b) {
            return game->getCardManager()->compareCards(a, b);
        });

        for (size_t i = 1; i < hand.size(); ++i) {
            ASSERT_TRUE(game->getCardManager()->compareCards(hand[i - 1], hand[i]))
                << "Player " << client << " has unsorted or invalid cards in hand";
        }
    }
}

// Test to validate player roles and Battle initialization consistency
TEST_F(DurakGameTest, TestGameConstructor_PlayerRolesAndBattleConsistency) {
    ASSERT_NE(game->getCurrentBattle(), nullptr)
        << "Battle object was not initialized during game construction";

    // Ensure roles match between game and battle
    const auto& player_roles = game->getPlayerRoles();
    for (const auto& [id, role] : player_roles) {
        EXPECT_EQ(game->getCurrentBattle()->getPlayerRole(id), role)
            << "Role mismatch for player " << id << " between Game and Battle";
    }
}
