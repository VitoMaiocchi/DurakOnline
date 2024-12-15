#include <gtest/gtest.h>
#include "../Server/include/server.hpp"
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>


// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

std::set<unsigned int> DurakServer::clients;
std::map<unsigned int, Player> DurakServer::players_map;


// Test fixture for the Battle class, used for testing functions from battle and cardmanager
class DurakBattleTest : public ::testing::Test {
protected:
    CardManager* card_manager; // Use a pointer to reset easily
    Battle* battle;
    BattleType btype;
    std::map<ClientID, PlayerRole> players_bs;
    std::vector<ClientID> clients;
    std::set<ClientID> finished;

    // Method to initialize the game with a dynamic number of players
    void initializeGame(size_t num_players) {
        //delete card_manager;
        //delete battle;

        // Ensure the number of players is between 2 and 6
        ASSERT_GE(num_players, 2) << "Minimum number of players is 3.";
        ASSERT_LE(num_players, 6) << "Maximum number of players is 6.";

        // Create player IDs and roles based on the number of players
        clients.clear();
        players_bs.clear();

        for (size_t i = 1; i <= num_players; ++i) {
            clients.push_back(i);
        }

        if (num_players >= 3) {
            players_bs[1] = ATTACKER;
            players_bs[2] = DEFENDER;
            players_bs[3] = CO_ATTACKER;
        }

        if (num_players > 3) {
            players_bs[4] = IDLE;
        }

        if (num_players > 4) {
            players_bs[5] = IDLE;
        }

        if (num_players > 5) {
            players_bs[6] = IDLE;
        }

        // Initialize CardManager and Battle
        std::set<ClientID> client_set(clients.begin(), clients.end());
        card_manager = new CardManager(client_set);
        battle = new Battle(BATTLETYPE_NORMAL, players_bs, *card_manager, finished);
    }

    // Override SetUp to default to 3 players
    void SetUp() override {
        initializeGame(3); // Default initialization with 3 players
    }

    void TearDown() override {
        delete card_manager; // Clean up to avoid memory leaks
        delete battle;
    }
};

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

// Test case: Card manager update
TEST(CardManagerTest, CardUpdate) {
    std::set<unsigned int> players = {1, 2, 3};
    CardManager card_manager(players);

    EXPECT_NO_THROW(card_manager.cardUpdate());
}

// Test compare cards function for multiple card pairs
TEST_F(DurakBattleTest, TestCompareCards) {
    // Set up the trump suit and the base card to compare against
    card_manager->setTrump(SUIT_HEARTS);
    Card cardPlayed(RANK_QUEEN, SUIT_CLUBS); // Base card

    // Define test cases with the card to compare and the expected result
    struct CompareTestCase {
        Card cardToCompare;
        bool expectedResult;
        std::string description; // Additional information for debugging
    };

    std::vector<CompareTestCase> testCases = {
        {Card(RANK_KING, SUIT_CLUBS), true, "Same suit, higher rank"}, // card1
        {Card(RANK_ACE, SUIT_CLUBS), true, "Same suit, higher rank"}, // card2
        {Card(RANK_FIVE, SUIT_CLUBS), false, "Same suit, lower rank"}, // card3
        {Card(RANK_FIVE, SUIT_HEARTS), true, "Trump suit, lower rank"}, // card4
        {Card(RANK_FIVE, SUIT_SPADES), false, "Different non-trump suit, lower rank"}, // card5
        {Card(RANK_KING, SUIT_SPADES), false, "Different non-trump suit, higher rank"} // card6
    };

    // Run the test cases
    for (const auto& testCase : testCases) {
        EXPECT_EQ(card_manager->compareCards(cardPlayed, testCase.cardToCompare), testCase.expectedResult);
    }
}

// Test DrawFromMiddleFunction
TEST_F(DurakBattleTest, TestDrawFromMiddle) {
    // Initialize the game with 4 players
    initializeGame(4);

    // Ensure the deck has enough cards for testing
    card_manager->fillDeck();
    unsigned int initial_deck_size = card_manager->getNumberOfCardsOnDeck();

    // Set up player hands: some with less than 6 cards, some with 6 or more cards
    std::vector<ClientID> players = {1, 2, 3, 4};
    //Player 1 has 6 cards
    card_manager->clearPlayerHand(2); // Player 2: 0 cards
    card_manager->clearPlayerHand(3); // Player 3: 0 cards
    //Player 4 has 6 cards

    EXPECT_EQ(card_manager->getNumberOfCardsInHand(1), 6);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(2), 0);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(3), 0);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(4), 6);


    //Add 3 cards to player 2, 4 cards to player 3 and 5 cards to player 4
    card_manager->addCardToPlayerHandFromDeck(2);
    card_manager->addCardToPlayerHandFromDeck(2);
    card_manager->addCardToPlayerHandFromDeck(2);
    card_manager->addCardToPlayerHandFromDeck(3);
    card_manager->addCardToPlayerHandFromDeck(3);
    card_manager->addCardToPlayerHandFromDeck(3);
    card_manager->addCardToPlayerHandFromDeck(3);
    card_manager->addCardToPlayerHandFromDeck(3);
    card_manager->addCardToPlayerHandFromDeck(4);
    card_manager->addCardToPlayerHandFromDeck(4);
    card_manager->addCardToPlayerHandFromDeck(4);
    card_manager->addCardToPlayerHandFromDeck(4);
    card_manager->addCardToPlayerHandFromDeck(4);

    // Ensure initial hand sizes are as expected
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(1), 6);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(2), 3);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(3), 5);
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(4), 11);

    // Draw cards for all players
    for (ClientID player : players) {
        card_manager->drawFromMiddle(player);
    }

    // Check that Player 1 has exactly 6 cards now (didnt draw any)
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(1), 6);

    // Check that Player 2 has exactly 6 cards now
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(2), 6);

    // Check that Player 3 still has 6 cards (no cards drawn since already has 6)
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(3), 6);

    // Check that Player 4 has exactly 11 cards (didn't draw any)
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(4), 11);

}

TEST_F(DurakBattleTest, TestPickUpWithRandomMiddleCards) {
    initializeGame(3);

    // Put random cards in the middle
    card_manager->clearMiddle();
    

    // Get initial cards in the middle and deck size
    unsigned int initial_middle_cards = card_manager->putRandomCardsInMiddle();
    unsigned int initial_player_hand = card_manager->getNumberOfCardsInHand(2);
    std::cout << "Cards in hand" << initial_player_hand << std::endl;
    std::cout << "initial_middle_cards" << initial_middle_cards << std::endl;
    // Call pickUp for Player 2
    card_manager->pickUp(2);

    // Verify that all cards from the middle are moved to Player 2's hand
    EXPECT_EQ(card_manager->getNumberOfCardsInHand(2), initial_player_hand + initial_middle_cards);

    // Verify that the middle is empty
    auto middle = card_manager->getMiddle();
    for (const auto& slot : middle) {
        EXPECT_FALSE(slot.first.has_value());
        EXPECT_FALSE(slot.second.has_value());
    }
}

TEST_F(DurakBattleTest, TestClearMiddleWithRandomCards) {
    initializeGame(4);

    // Make sure middle is empty
    card_manager->clearMiddle();

    // Fill middle with random cards
    unsigned int initial_middle_cards = card_manager->putRandomCardsInMiddle();

    //Clear Middle
    card_manager->clearMiddle();
    // Verify that the middle is empty
    auto middle = card_manager->getMiddle();
    for (const auto& slot : middle) {
        EXPECT_FALSE(slot.first.has_value());
        EXPECT_FALSE(slot.second.has_value());
    }

    // Verify the number of discarded cards matches the number of cards cleared
    //EXPECT_EQ(card_manager->getNumberDiscardedCards(), initial_middle_cards);
}

TEST_F(DurakBattleTest, TestIsValidMove_AttackerValid) {
    // Initialize a game with 3 players
    initializeGame(3);

    // Set up a card for the attacker
    Card cardPlayed(RANK_QUEEN, SUIT_HEARTS);
    ClientID attacker_id = 1;
    CardSlot slot = CARDSLOT_1;

    // Add the card to the attacker's hand
    card_manager->addCardToPlayerHand(attacker_id, cardPlayed);

    // Validate that the move is valid for the attacker
    EXPECT_TRUE(battle->isValidMove(cardPlayed, attacker_id, slot));
}

TEST_F(DurakBattleTest, TestIsValidMove_DefenderValid) {
    // Initialize a game with 3 players
    initializeGame(3);

    // Setup for the attacker and defender
    Card attackCard(RANK_TEN, SUIT_HEARTS);
    ClientID attacker_id = 1;
    ClientID defender_id = 2;
    CardSlot slot_defend = CARDSLOT_1;  // Defender's response slot
    CardSlot slot_passon1 = CARDSLOT_2;
    CardSlot slot_passon2 = CARDSLOT_6;

    // Define test cards
    std::vector<std::pair<Card, bool>> defenseMoves = {
        {Card(RANK_JACK, SUIT_HEARTS), true},  // Same suit, higher rank
        {Card(RANK_QUEEN, SUIT_HEARTS), true}, // Same suit, higher rank
        {Card(RANK_KING, SUIT_HEARTS), true},  // Same suit, higher rank
        {Card(RANK_FIVE, SUIT_HEARTS), false}, // Same suit, lower rank
        {Card(RANK_FIVE, SUIT_DIAMONDS), false}, // Non-trump, lower rank
        {Card(RANK_FIVE, SUIT_SPADES), true},  // Trump suit, lower rank
        {Card(RANK_ACE, SUIT_SPADES), true}    // Trump suit, higher rank
    };

    std::vector<std::pair<Card, bool>> passMoves = {
        {Card(RANK_TEN, SUIT_CLUBS), true},    // Same rank, different suit
        {Card(RANK_TEN, SUIT_DIAMONDS), true}, // Same rank, different suit
        {Card(RANK_KING, SUIT_HEARTS), false}, // Same suit, higher rank
        {Card(RANK_FIVE, SUIT_DIAMONDS), false}, // Non-trump, lower rank
        {Card(RANK_ACE, SUIT_SPADES), false}   // Trump suit, higher rank
    };

    // Set trump suit
    card_manager->setTrump(SUIT_SPADES);

    // Place the attack card in the middle to simulate an ongoing battle
    card_manager->addCardToPlayerHand(attacker_id, attackCard);
    card_manager->attackCard(attackCard, attacker_id);

    // Add all test cards to the defender's hand
    for (const auto& move : defenseMoves) {
        card_manager->addCardToPlayerHand(defender_id, move.first);
    }
    for (const auto& move : passMoves) {
        card_manager->addCardToPlayerHand(defender_id, move.first);
    }

    // Test defense moves
    for (const auto& [card, expected] : defenseMoves) {
        EXPECT_EQ(battle->isValidMove(card, defender_id, slot_defend), expected);
    }

    // Test pass-on moves
    for (const auto& [card, expected] : passMoves) {
        CardSlot testSlot = (card.rank == RANK_TEN && card.suit == SUIT_CLUBS) ? slot_passon1 : slot_passon2;
        EXPECT_EQ(battle->isValidMove(card, defender_id, testSlot), expected);
    }
}

TEST_F(DurakBattleTest, TestMovePlayerRolesMultipleConfigurations) {
    // Define a helper data structure for test cases
    struct TestCase {
        size_t num_players; // Number of players
        std::map<ClientID, PlayerRole> initial_roles; // Initial roles
        std::vector<std::map<ClientID, PlayerRole>> expected_states; // Expected states after each move
    };

    // Define the test cases
    std::vector<TestCase> test_cases = {
        // Test case 1: 2 players (no CO_ATTACKER)
        {
            2,
            {{1, ATTACKER}, {2, DEFENDER}},
            {
                {{1, DEFENDER}, {2, ATTACKER}},
                {{1, ATTACKER}, {2, DEFENDER}}
            }
        },
        // Test case 2: 3 players
        {
            3,
            {{1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}},
            {
                {{1, CO_ATTACKER}, {2, ATTACKER}, {3, DEFENDER}},
                {{1, DEFENDER}, {2, CO_ATTACKER}, {3, ATTACKER}},
                {{1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}}
            }
        },
        // Test case 3: 4 players
        {
            4,
            {{1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}, {4, IDLE}},
            {
                {{1, IDLE}, {2, ATTACKER}, {3, DEFENDER}, {4, CO_ATTACKER}},
                {{1, CO_ATTACKER}, {2, IDLE}, {3, ATTACKER}, {4, DEFENDER}},
                {{1, DEFENDER}, {2, CO_ATTACKER}, {3, IDLE}, {4, ATTACKER}}
            }
        },
        // Test case 4: 5 players
        {
            5,
            {{1, IDLE}, {2, IDLE}, {3, ATTACKER}, {4, DEFENDER}, {5, CO_ATTACKER}},
            {
                {{1, CO_ATTACKER}, {2, IDLE}, {3, IDLE}, {4, ATTACKER}, {5, DEFENDER}},
                {{1, DEFENDER}, {2, CO_ATTACKER}, {3, IDLE}, {4, IDLE}, {5, ATTACKER}},
                {{1, ATTACKER}, {2, DEFENDER}, {3, CO_ATTACKER}, {4, IDLE}, {5, IDLE}}
            }
        },
        // Test case 5: 6 players
        {
            6,
            {{1, IDLE}, {2, ATTACKER}, {3, DEFENDER}, {4, CO_ATTACKER}, {5, IDLE}, {6, IDLE}},
            {
                {{1, IDLE}, {2, IDLE}, {3, ATTACKER}, {4, DEFENDER}, {5, CO_ATTACKER}, {6, IDLE}},
                {{1, IDLE}, {2, IDLE}, {3, IDLE}, {4, ATTACKER}, {5, DEFENDER}, {6, CO_ATTACKER}},
                {{1, CO_ATTACKER}, {2, IDLE}, {3, IDLE}, {4, IDLE}, {5, ATTACKER}, {6, DEFENDER}}
            }
        }
    };

    // Iterate over each test case
    for (const auto& test_case : test_cases) {
        initializeGame(test_case.num_players);

        // Set initial roles explicitly
        battle->setPlayerRoles(test_case.initial_roles);

        // Add cards to ensure no players are removed as "finished"
        for (const auto& [id, role] : test_case.initial_roles) {
            if (role != FINISHED) {
                Card card(RANK_ACE, SUIT_SPADES);
                card_manager->addCardToPlayerHand(id, card);
            }
        }

        // Verify each expected state after successive calls to movePlayerRoles
        for (const auto& expected_state : test_case.expected_states) {
            battle->movePlayerRoles();
            const auto& current_roles = battle->getPlayerRolesMap();

            std::cout << "Expected state:\n";
            for (const auto& [id, role] : expected_state) {
                std::cout << "Player " << id << " -> Role " << role << "\n";
            }

            std::cout << "Actual state:\n";
            for (const auto& [id, role] : current_roles) {
                std::cout << "Player " << id << " -> Role " << role << "\n";
            }

            // Compare the current roles with the expected state
            for (const auto& [player_id, role] : expected_state) {
                EXPECT_EQ(current_roles.at(player_id), role)
                    << "Failed for player ID " << player_id
                    << ". Expected role: " << role
                    << ", Actual role: " << current_roles.at(player_id);
            }
        }
    }
}


/*
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

TEST_F(DurakBattleTest, TestHandleCardEvent_PasssingOn){
    // ::testing::internal::CaptureStdout();
    // Mock setup for passing on the attack
    Card attackCard(RANK_QUEEN, SUIT_CLUBS); 
    Card passOnCard(RANK_QUEEN, SUIT_HEARTS);  

    std::vector<Card> cards = { passOnCard };

    ClientID attacker_id = 1;        // Attacker ID
    ClientID defender_id = 2;        // Defender ID
    CardSlot slot = CARDSLOT_2; // Slot for the defender's response

    // Place the attack card in the middle to simulate an ongoing battle
    // card_manager.placeAttackCard(attackCard, slot);
    card_manager->addCardToPlayerHand(attacker_id, attackCard);
    card_manager->addCardToPlayerHand(defender_id, passOnCard);

    battle->attack(attacker_id, attackCard);
    // ASSERT_TRUE(card_manager.attackCard(attackCard, attacker_id));
    // Call isValidMove to check the defender's move
    bool result = battle->handleCardEvent(cards, defender_id, slot);

    // std::string output = ::testing::internal::GetCapturedStdout();  // Stop capturing and get output
    // std::cout << "Captured output: " << output << std::endl;

    EXPECT_TRUE(result);

}*/
