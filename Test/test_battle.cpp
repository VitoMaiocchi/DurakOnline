#include <gtest/gtest.h>
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>


// Test fixture for the Battle class
class DurakBattleTest : public ::testing::Test {
protected:
    CardManager* card_manager; // Use a pointer to reset easily
    Battle* battle;
    std::map<ClientID, PlayerRole> players_bs;
    std::vector<ClientID> clients;

    void SetUp() override {
        clients = {1, 2, 3}; // Reinitialize clients
        players_bs = {{1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}}; // Reinitialize roles
        card_manager = new CardManager(clients); // Create a new CardManager
        battle = new Battle(false, players_bs, *card_manager); // Create a new Battle
    }

    void TearDown() override {
        delete card_manager; // Clean up to avoid memory leaks
        delete battle;
    }
};

TEST_F(DurakBattleTest, TestIsValidMove_AttackerValid) {
    // Set up a mock Card, player ID, and CardSlot as required by isValidMove
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS); // Sample card for the move
    ClientID player_id = 1;                        // ID of the attacker
    CardSlot slot = CARDSLOT_1;               // Sample slot

    card_manager->addCardToPlayerHand(player_id, cardPlayed);
    // Call isValidMove and check the result for a valid attack
    bool result = battle->isValidMove(cardPlayed, player_id, slot);

    // Validate that the move is initially valid for the attacker
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestIsValidMove_DefenderValid) {
    // ::testing::internal::CaptureStdout();
    // Mock setup for a defender’s valid move
    Card attackCard(RANK_QUEEN, SUIT_HEARTS); // Attacker's card on the table
    Card defendCard(RANK_KING, SUIT_HEARTS);  // Defender's card, assumed valid for defense

    int attacker_id = 1;        // Attacker ID
    int defender_id = 2;        // Defender ID
    CardSlot slot = CARDSLOT_1_TOP; // Slot for the defender's response

    // Place the attack card in the middle to simulate an ongoing battle
    // card_manager.placeAttackCard(attackCard, slot);
    card_manager->addCardToPlayerHand(attacker_id, attackCard);
    card_manager->addCardToPlayerHand(defender_id, defendCard);

    card_manager->attackCard(attackCard, attacker_id);

    // ASSERT_TRUE(card_manager.attackCard(attackCard, attacker_id));
    // Call isValidMove to check the defender's move
    bool result = battle->isValidMove(defendCard, defender_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;
    // Check if the defender's move is valid
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestIsValidMove_AttackLimitExceeded) {
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS);
    ClientID player_id = 1;
    CardSlot slot = CARDSLOT_1;

    card_manager->addCardToPlayerHand(player_id, cardPlayed);
    // Direct access due to friend declaration
    battle->setCurrAttacks(battle->getMaxAttacks());

    bool result = battle->isValidMove(cardPlayed, player_id, slot);
    EXPECT_FALSE(result);
}

TEST_F(DurakBattleTest, TestHandleCardEvent_Attacker) {
    // Test handleCardEvent with an attacker scenario
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS); // Attacker's card
    ClientID player_id = 1;                        // Attacker ID
    CardSlot slot = CARDSLOT_1;               // Slot for attack

    std::vector<Card> cards = { cardPlayed };

    card_manager->addCardToPlayerHand(player_id, cardPlayed);
    // Call handleCardEvent for an attack scenario
    bool result = battle->handleCardEvent(cards, player_id, slot);

    // Validate that handleCardEvent processes a valid attack
    EXPECT_TRUE(result);
}


TEST_F(DurakBattleTest, TestHandleCardEvent_Defender){
    // ::testing::internal::CaptureStdout();
    // Mock setup for a defender’s valid move
    Card attackCard(RANK_QUEEN, SUIT_HEARTS); // Attacker's card on the table
    Card defendCard(RANK_KING, SUIT_HEARTS);  // Defender's card, assumed valid for defense

    std::vector<Card> cards = { defendCard };

    int attacker_id = 1;        // Attacker ID
    int defender_id = 2;        // Defender ID
    CardSlot slot = CARDSLOT_1_TOP; // Slot for the defender's response

    // Place the attack card in the middle to simulate an ongoing battle
    // card_manager.placeAttackCard(attackCard, slot);
    card_manager->addCardToPlayerHand(attacker_id, attackCard);
    card_manager->addCardToPlayerHand(defender_id, defendCard);

    // card_manager.attackCard(attackCard, attacker_id);

    // ASSERT_TRUE(card_manager.attackCard(attackCard, attacker_id));
    battle->attack(attacker_id, attackCard);
    // battle.setAttacksToDefend(1);

    // Call isValidMove to check the defender's move
    bool result = battle->handleCardEvent(cards, defender_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;
    // Check if the defender's move is valid
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestHandleCardEvent_DefenderIsWrong){
    // ::testing::internal::CaptureStdout();
    // Mock setup for a defender’s valid move
    Card attackCard(RANK_QUEEN, SUIT_CLUBS); // Attacker's card on the table
    Card defendCard(RANK_KING, SUIT_HEARTS);  // Defender's card, assumed valid for defense

    std::vector<Card> cards = { defendCard };

    int attacker_id = 1;        // Attacker ID
    int defender_id = 2;        // Defender ID
    CardSlot slot = CARDSLOT_1_TOP; // Slot for the defender's response

    // Place the attack card in the middle to simulate an ongoing battle
    // card_manager.placeAttackCard(attackCard, slot);
    card_manager->addCardToPlayerHand(attacker_id, attackCard);
    card_manager->addCardToPlayerHand(defender_id, defendCard);

    card_manager->attackCard(attackCard, attacker_id);

    // ASSERT_TRUE(card_manager.attackCard(attackCard, attacker_id));
    // Call isValidMove to check the defender's move
    bool result = battle->handleCardEvent(cards, defender_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;
    // Check if the defender's move is valid
    if(card_manager->getTrump() == SUIT_HEARTS){
        EXPECT_TRUE(result);
    }
    else{
        EXPECT_FALSE(result); 
    }

}

TEST_F(DurakBattleTest, TestHandleCardEvent_MultipleAttacks){
    // ::testing::internal::CaptureStdout();
    // Mock setup for a attackers multiple cards play
    Card attackCard1(RANK_QUEEN, SUIT_CLUBS); // Attacker's card on the table
    Card attackCard2(RANK_QUEEN, SUIT_HEARTS); // Attacker's card on the table

    std::vector<Card> cards = { attackCard1, attackCard2};

    ClientID attacker_id = 1;        // Attacker ID
    CardSlot slot = CARDSLOT_1; // Slot doesnt matter

    card_manager->addCardToPlayerHand(attacker_id, attackCard1);
    card_manager->addCardToPlayerHand(attacker_id, attackCard2);


    // Call isValidMove to check the attacker's move
    bool result = battle->handleCardEvent(cards, attacker_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;
    // Check if the defender's move is valid
    EXPECT_TRUE(result);
}

TEST_F(DurakBattleTest, TestHandleCardEvent_MultipleCOAttacks){
    // ::testing::internal::CaptureStdout();
    // Mock setup for a attackers multiple cards play
    Card attackCard1(RANK_JACK, SUIT_CLUBS); // Attacker's card on the table
    Card attackCard2(RANK_JACK, SUIT_HEARTS); // Attacker's card on the table

    Card cardOnTable(RANK_JACK, SUIT_SPADES);
    card_manager->placeAttackCard(cardOnTable, CARDSLOT_1);


    std::vector<Card> cards = { attackCard1, attackCard2};

    ClientID attacker_id = 3;        // Attacker ID
    CardSlot slot = CARDSLOT_2; // Slot doesnt matter

    card_manager->addCardToPlayerHand(attacker_id, attackCard1);
    card_manager->addCardToPlayerHand(attacker_id, attackCard2);

    // Call isValidMove to check the attacker's move
    bool result = battle->handleCardEvent(cards, attacker_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;
    // Check if the defender's move is valid
    EXPECT_TRUE(result);
}
