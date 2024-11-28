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
    std::optional<Suit> trump = std::make_optional<Suit>(game->getCardManager()->getTrump());
    ASSERT_NE(trump, std::nullopt)
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

        // // Ensure cards are unique and sorted (optional sorting for validation)
        // std::sort(hand.begin(), hand.end(), [&](const Card& a, const Card& b) {
        //     return game->getCardManager()->compareCards(a, b);
        // });

        std::cout << "hand:\n";
        for(Card card : hand){
            std::cout << "card:\t" << card.rank << "-" << card.suit << std::endl;
        }
        //check that no card is the same in the hand
        for (size_t i = 1; i < hand.size(); ++i) {
            for(size_t j = i + 1; j < hand.size();++j){
                ASSERT_TRUE(hand[j] != hand[j + 1]);
            }
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
// Test to validate the correctness of defender and second attacker logic
TEST_F(DurakGameTest, TestGameConstructor_DefenderAndSecondAttacker) {
    // Retrieve the player roles from the game
    const auto& player_roles = game->getPlayerRoles();

    // Find the first attacker
    ClientID first_attacker = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == ATTACKER) {
            first_attacker = id;
            break;
        }
    }
    ASSERT_NE(first_attacker, -1) << "First attacker was not determined";

    // Determine the expected defender
    auto attacker_it = std::find(clients.begin(), clients.end(), first_attacker);
    ASSERT_NE(attacker_it, clients.end()) << "First attacker not found in player list";

    ClientID expected_defender = (attacker_it + 1 != clients.end()) 
                                    ? *(attacker_it + 1)
                                    : clients.front();

    // Find the defender in the roles
    ClientID actual_defender = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == DEFENDER) {
            actual_defender = id;
            break;
        }
    }
    ASSERT_NE(actual_defender, -1) << "Defender was not determined";

    // Validate the defender assignment
    EXPECT_EQ(actual_defender, expected_defender) 
        << "Defender role assignment is incorrect";

    // Determine the expected second attacker
    auto defender_it = std::find(clients.begin(), clients.end(), actual_defender);
    ASSERT_NE(defender_it, clients.end()) << "Defender not found in player list";

    ClientID expected_second_attacker = (defender_it + 1 != clients.end()) 
                                            ? *(defender_it + 1)
                                            : clients.front();

    // Find the second attacker in the roles
    ClientID actual_second_attacker = -1;
    for (const auto& [id, role] : player_roles) {
        if (role == CO_ATTACKER) {
            actual_second_attacker = id;
            break;
        }
    }
    ASSERT_NE(actual_second_attacker, -1) << "Second attacker was not determined";

    // Validate the second attacker assignment
    EXPECT_EQ(actual_second_attacker, expected_second_attacker)
        << "Second attacker role assignment is incorrect";
}
