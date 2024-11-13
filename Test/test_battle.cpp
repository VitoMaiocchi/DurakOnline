#include <gtest/gtest.h>
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>


// Test fixture for the Battle class
class DurakBattleTest : public ::testing::Test {
protected:
    CardManager card_manager;
    std::vector<std::pair<int, PlayerRole>> players_bs;
    Battle battle;

    // Constructor for setting up the test fixture with initialized components
    DurakBattleTest()
        : card_manager(3),                         // Initialize CardManager for 2 players
          players_bs({ {1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}}), // Sample player setup
          battle(false, players_bs, card_manager)    // Initialize Battle with first_battle = true
    {
        card_manager.fillDeck();
    }

    void SetUp() override {
        // Additional setup if necessary
    }

    void TearDown() override {
        // Clean up resources if necessary
    }
};

TEST_F(DurakBattleTest, TestIsValidMove_AttackerValid) {
    // Set up a mock Card, player ID, and CardSlot as required by isValidMove
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS); // Sample card for the move
    int player_id = 1;                        // ID of the attacker
    CardSlot slot = CARDSLOT_1;               // Sample slot

    // Call isValidMove and check the result for a valid attack
    bool result = battle.isValidMove(cardPlayed, player_id, slot);

    // Validate that the move is initially valid for the attacker
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestIsValidMove_DefenderValid) {
    // Mock setup for a defender’s valid move
    Card attackCard(RANK_QUEEN, SUIT_HEARTS); // Attacker's card on the table
    Card defendCard(RANK_KING, SUIT_HEARTS);  // Defender's card, assumed valid for defense

    int attacker_id = 1;        // Attacker ID
    int defender_id = 2;        // Defender ID
    CardSlot slot = CARDSLOT_1; // Slot for the defender's response

    // Place the attack card in the middle to simulate an ongoing battle
    // card_manager.getMiddle().at(slot).first = attackCard;
    card_manager.attackCard(attackCard, 1);

    // Call isValidMove to check the defender's move
    bool result = battle.isValidMove(defendCard, defender_id, slot);

    // Check if the defender's move is valid
    EXPECT_TRUE(result);
}



TEST_F(DurakBattleTest, TestHandleCardEvent_Attacker) {
    // Test handleCardEvent with an attacker scenario
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS); // Attacker's card
    int player_id = 1;                        // Attacker ID
    CardSlot slot = CARDSLOT_1;               // Slot for attack

    std::vector<Card> cards = { cardPlayed };

    // Call handleCardEvent for an attack scenario
    bool result = battle.handleCardEvent(cards, player_id, slot);

    // Validate that handleCardEvent processes a valid attack
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestIsValidMove_AttackLimitExceeded) {
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS);
    int player_id = 1;
    CardSlot slot = CARDSLOT_1;

    // Direct access due to friend declaration
    battle.setCurrAttacks(battle.getMaxAttacks());

    bool result = battle.isValidMove(cardPlayed, player_id, slot);
    EXPECT_FALSE(result);
}
