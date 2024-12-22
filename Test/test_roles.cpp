#include <gtest/gtest.h>
#include "../Server/include/battle.hpp"
#include "../Server/include/card_manager.hpp"
#include <Networking/util.hpp>
#include "../include/server.hpp"

// Define static members
std::set<unsigned int> DurakServer::clients;
std::map<unsigned int, Player> DurakServer::players_map;


class DurakRoleTest : public ::testing::Test {
protected:
    CardManager* card_manager;
    Battle* battle;
    std::map<ClientID, PlayerRole> players_bs;
    std::vector<ClientID> clients;
    std::set<ClientID> finished;

    void SetUp() override {
        DurakServer::clients = {1, 2, 3, 4, 5, 6};
        for(unsigned int i = 1; i <= 6; ++i){
            DurakServer::players_map[i] = Player{"PlayerName", i, false};
        }
        initializeGame(6); // Default initialization with 6 players
    }

    void TearDown() override {
        delete card_manager;
        delete battle;
    }

    void initializeGame(size_t num_players) {
        clients.clear();
        players_bs.clear();

        for (size_t i = 1; i <= num_players; ++i) {
            clients.push_back(i);
        }

        players_bs[1] = ATTACKER;
        players_bs[2] = DEFENDER;
        players_bs[3] = CO_ATTACKER;
        players_bs[4] = IDLE;
        players_bs[5] = IDLE;
        players_bs[6] = IDLE;

        std::set<ClientID> client_set(clients.begin(), clients.end());
        card_manager = new CardManager(client_set);
        battle = new Battle(BATTLETYPE_NORMAL, players_bs, *card_manager, finished);
    }
};

// Case 1: Attacker finishes, defender picks up
TEST_F(DurakRoleTest, AttackerFinishesDefenderPicksUp) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(1); // Attacker finishes
    battle->movePlayerRoles();

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[3], DEFENDER);
    EXPECT_EQ(roles[4], CO_ATTACKER);
}

// Case 2: Attacker finishes, defender defends
TEST_F(DurakRoleTest, AttackerFinishesDefenderDefends) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(1); // Attacker finishes
    battle->movePlayerRoles();

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[3], DEFENDER);
    EXPECT_EQ(roles[4], CO_ATTACKER);
}

// Case 3: Defender passes on
TEST_F(DurakRoleTest, CoAttackerFinishes) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(3); // Co-attacker finishes
    battle->movePlayerRoles();

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

// Case 4: Defender finishes
TEST_F(DurakRoleTest, DefenderFinishes) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(2); // Defender finishes
    battle->movePlayerRoles();
    // battle->movePlayerRoles();

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[3], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

// Case 5: Co-attacker finishes, defender picks up
TEST_F(DurakRoleTest, CoAttackerFinishesDefenderPicksUp) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(3); // Co-attacker finishes
    battle->movePlayerRoles();
    battle->movePlayerRoles(); //because of the pick up

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[4], ATTACKER);
    EXPECT_EQ(roles[5], DEFENDER);
    EXPECT_EQ(roles[6], CO_ATTACKER);
}

// Case 6: Co-attacker finishes, defender defends
TEST_F(DurakRoleTest, CoAttackerFinishesDefenderDefends) {
    card_manager->eraseDeck();
    card_manager->clearPlayerHand(3); // Co-attacker finishes
    battle->movePlayerRoles();

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

// Case 7: Co-attacker finishes, defender tries to pass on (invalid)
// TEST_F(DurakRoleTest, CoAttackerFinishesDefenderInvalidPass) {
//     card_manager->clearPlayerHand(3); // Co-attacker finishes
//     Card card_in_middle = Card(RANK_ACE, SUIT_HEARTS);
//     Card card_to_play = Card(RANK_ACE,SUIT_CLUBS);
//     card_manager->addCardToPlayerHand(2, card_to_play);
//     card_manager->placeAttackCard(card_in_middle, CARDSLOT_1);
//     // battle->movePlayerRoles();

//     // Validate invalid pass-on attempt
//     // std::unordered_set<Card>& cards, ClientID player_id, CardSlot slot
//     std::unordered_set<Card> cards = {(RANK_ACE, SUIT_CLUBS)};
//     EXPECT_FALSE(battle->passOn(cards, 2, CARDSLOT_3));
// }

TEST_F(DurakRoleTest, AttackerFinishesDefenderPassOn){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    card_manager->clearPlayerHand(1);
    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    card_manager->attackCard(cardA, 1);
    Card cardD = Card(RANK_EIGHT, SUIT_SPADES);
    card_manager->addCardToPlayerHand(2, cardD);
    std::unordered_set<Card> cards = {cardD};
    battle->passOn(cards, 2, CARDSLOT_2);

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles.size(), 5);
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[3], DEFENDER);
    EXPECT_EQ(roles[4], CO_ATTACKER);
}

TEST_F(DurakRoleTest, DefenderFinishesDefenderPassOn){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    card_manager->clearPlayerHand(2);
    Card cardD = Card(RANK_EIGHT, SUIT_SPADES);
    card_manager->addCardToPlayerHand(2, cardD);

    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    card_manager->attackCard(cardA, 1);

    std::unordered_set<Card> cards = {cardD};
    battle->passOn(cards, 2, CARDSLOT_2);

    auto roles = battle->getPlayerRolesMap();
    //it shouldnt remove the player 2 yet
    // Validate role updates
    EXPECT_EQ(roles.size(), 6);
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[3], DEFENDER);
    EXPECT_EQ(roles[4], CO_ATTACKER);

    //it must be that 2 stays attacker but is not removed yet and then when 3 defends everything
    //it should move the roles

    battle->movePlayerRoles(); //after the turn
    roles = battle->getPlayerRolesMap();
    EXPECT_EQ(roles.size(), 5);
    EXPECT_EQ(roles[3], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

TEST_F(DurakRoleTest, AttackerFinishesDefenderPassOnMoreExplicit){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    card_manager->clearPlayerHand(1);
    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    std::vector<Card> cardsA = {cardA};
    Card cardD = Card(RANK_EIGHT, SUIT_SPADES);
    card_manager->addCardToPlayerHand(2, cardD);
    std::vector<Card> cardsD = {cardD};

    // message of handleCardEvent for the attacker
    // message of handleCardEvent for defender
    battle->handleCardEvent(cardsA, 1, CARDSLOT_1);
    battle->handleCardEvent(cardsD, 2, CARDSLOT_2);

    /*                              check this first
    1 -> ATTACKER  -> finsihes  -> removed
    2 -> DEFENDER  -> passes ON -> new ATTACKER   -> after battle -> IDLE
    3 -> COATTACKER             -> new DEFENDER                   -> ATTACKER
    4 -> IDLE                   -> new COATTACKER                 -> DEFENDER
    5 -> IDLE                   -> IDLE                           -> COATTACKER
    6 -> IDLE                   -> IDLE                           -> IDLE  
     */

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles.size(), 5);
    EXPECT_EQ(roles[2], ATTACKER);
    EXPECT_EQ(roles[3], DEFENDER);
    EXPECT_EQ(roles[4], CO_ATTACKER);
}

TEST_F(DurakRoleTest, DefenderFinishesMoreExplicit){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    std::vector<Card> cardsA = {cardA};
    card_manager->clearPlayerHand(2);
    Card cardD = Card(RANK_NINE, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(2, cardD);
    std::vector<Card> cardsD = {cardD};

    // message of handleCardEvent for the attacker
    // message of handleCardEvent for defender
    battle->handleCardEvent(cardsA, 1, CARDSLOT_1);
    battle->handleCardEvent(cardsD, 2, CARDSLOT_1_TOP);

    battle->handleActionEvent(1, CLIENTACTION_OK);
    battle->handleActionEvent(3, CLIENTACTION_OK);

    /*                              check this first
    1 -> ATTACKER               -> IDLE
    2 -> DEFENDER               -> finishes    
    3 -> COATTACKER             -> new ATTACKER                
    4 -> IDLE                   -> new DEFENDER                
    5 -> IDLE                   -> new COATTACKER              
    6 -> IDLE                   -> IDLE                        
     */

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles.size(), 5);
    EXPECT_EQ(roles[3], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

TEST_F(DurakRoleTest, AttackerAndDefenderPlayerFinish){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    card_manager->clearPlayerHand(1);
    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    std::vector<Card> cardsA = {cardA};
    card_manager->clearPlayerHand(2);
    Card cardD = Card(RANK_NINE, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(2, cardD);
    std::vector<Card> cardsD = {cardD};

    // message of handleCardEvent for the attacker
    // message of handleCardEvent for defender
    battle->handleCardEvent(cardsA, 1, CARDSLOT_1);
    battle->handleCardEvent(cardsD, 2, CARDSLOT_1_TOP);

    battle->handleActionEvent(1, CLIENTACTION_OK);
    battle->handleActionEvent(3, CLIENTACTION_OK);

    /*                              check this first
    1 -> ATTACKER               -> finishes
    2 -> DEFENDER               -> finishes    
    3 -> COATTACKER             -> new ATTACKER                
    4 -> IDLE                   -> new DEFENDER                
    5 -> IDLE                   -> new COATTACKER              
    6 -> IDLE                   -> IDLE                        
     */

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles.size(), 4);
    EXPECT_EQ(roles[3], ATTACKER);
    EXPECT_EQ(roles[4], DEFENDER);
    EXPECT_EQ(roles[5], CO_ATTACKER);
}

TEST_F(DurakRoleTest, AttackerCoattackerAndDefenderFinish){
    card_manager->eraseDeck();
    card_manager->clearMiddle();
    card_manager->clearPlayerHand(1);
    Card cardA = Card(RANK_EIGHT, SUIT_CLUBS);
    card_manager->addCardToPlayerHand(1, cardA);
    std::vector<Card> cardsA = {cardA};

    card_manager->clearPlayerHand(2);
    Card cardD1 = Card(RANK_NINE, SUIT_CLUBS);
    Card cardD2 = Card(RANK_TEN, SUIT_HEARTS);
    card_manager->addCardToPlayerHand(2, cardD1);
    std::vector<Card> cardsD = {cardD1};
    std::vector<Card> cardsD2 = {cardD2};

    card_manager->clearPlayerHand(3);
    Card cardC = Card(RANK_NINE, SUIT_HEARTS);
    card_manager->addCardToPlayerHand(3,cardC);
    std::vector<Card> cardsC = {cardC};

    // message of handleCardEvent for the attacker
    // message of handleCardEvent for defender
    battle->handleCardEvent(cardsA, 1, CARDSLOT_1);
    battle->handleCardEvent(cardsD, 2, CARDSLOT_1_TOP);
    battle->handleCardEvent(cardsC, 3, CARDSLOT_2);
    battle->handleCardEvent(cardsD2, 2, CARDSLOT_2_TOP);

    battle->handleActionEvent(1, CLIENTACTION_OK);
    battle->handleActionEvent(3, CLIENTACTION_OK);

    /*                              check this first
    1 -> ATTACKER               -> finishes
    2 -> DEFENDER               -> finishes    
    3 -> COATTACKER             -> new ATTACKER                
    4 -> IDLE                   -> new DEFENDER                
    5 -> IDLE                   -> new COATTACKER              
    6 -> IDLE                   -> IDLE                        
     */

    auto roles = battle->getPlayerRolesMap();

    // Validate role updates
    EXPECT_EQ(roles.size(), 3);
    EXPECT_EQ(roles[4], ATTACKER);
    EXPECT_EQ(roles[5], DEFENDER);
    EXPECT_EQ(roles[6], CO_ATTACKER);
}