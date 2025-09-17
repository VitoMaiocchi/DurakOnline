#include "test_util.hpp"

/*

the first part is attackCard 

second part will be defendCard

third part will be coattackCard

*/
//testing attack card, but calling cardEvent to test both at same time
TEST(cardEvent, InvalidAttacker2Cards){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_THREE, SUIT_HEARTS});

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), false);
    EXPECT_EQ(middle[1].has_value(), false);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_FIRST_ATTACK);
}

TEST(cardEvent, Attacker2Cards){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 4);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}


TEST(cardEvent, Attacker1Card){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_THREE, SUIT_HEARTS});

    auto it = hands[attacker_idx].find(card1);
    if(it == hands[attacker_idx].end()){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }

    hands[attacker_idx].insert(card1);

    std::unordered_set<Card> cards = {card1};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), false);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 5);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, InvalidAttacker2CardsWhenAlready1OnField){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_THREE, SUIT_CLUBS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    // middle[CARDSLOT_1]
    placeCard(attacker_idx, cardmiddle, s); //place one card as setup in the middle -> first slot
    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set
    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), false);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker2CardsWith1OnField){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 4);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker2CardsWith2OnFieldAndTwoSameRank){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), true);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 4);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, InvalidAttacker2CardsWith2OnFieldAndTwoSameRank){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_THREE, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, InvalidAttacker2CardsWith2OnFieldAndTwoDifferentRank){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_FOUR, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, ValidAttacker2CardsWith2OnFieldAndTwoDifferentRank){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_THREE, SUIT_CLUBS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), true);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 4);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker2CardsWith2OnFieldAndDefenderOnlyHasTHREECARDSinHand){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    //change phase for the actual scenario to happen
    s.battle_type = BATTLETYPE_NORMAL;
    //remove 3 of defenders cards
    auto it_first = hands[defender_idx].begin();
    auto it_last = std::next(it_first); it_last = std::next(it_last); it_last = std::next(it_last);
    hands[defender_idx].erase(it_first, it_last);

    std::cout << "cheking the size of the def hand, its: " << hands[defender_idx].size() << std::endl;
    // hands[defender_idx].erase();


    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1, card2};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), false);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker1CardWith2OnFieldAndDefenderOnlyHasTHREECARDSinHand){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    //change phase for the actual scenario to happen
    s.battle_type = BATTLETYPE_NORMAL;
    //remove 3 of defenders cards
    auto it_first = hands[defender_idx].begin();
    auto it_last = std::next(it_first); it_last = std::next(it_last); it_last = std::next(it_last);
    hands[defender_idx].erase(it_first, it_last);

    std::cout << "cheking the size of the def hand, its: " << hands[defender_idx].size() << std::endl;
    // hands[defender_idx].erase();


    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);

    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), false);
    EXPECT_EQ(middle[4].has_value(), false);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 5);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker1CardWith5InMiddleBATTLEPHASEFIRSTATTACK){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    //change phase for the actual scenario to happen
    s.battle_type = BATTLETYPE_FIRST;

    std::cout << "cheking the size of the def hand, its: " << hands[defender_idx].size() << std::endl;
    // hands[defender_idx].erase();


    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card cardmiddle3 = Card({RANK_THREE, SUIT_CLUBS});
    Card cardmiddle4 = Card({RANK_THREE, SUIT_DIAMONDS});
    Card cardmiddle5 = Card({RANK_THREE, SUIT_SPADES});

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);
    placeCard(attacker_idx, cardmiddle3, s);
    placeCard(attacker_idx, cardmiddle4, s);
    placeCard(attacker_idx, cardmiddle5, s);


    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), true);
    EXPECT_EQ(middle[4].has_value(), true);
    EXPECT_EQ(middle[5].has_value(), false);


    EXPECT_EQ(hands[attacker_idx].size(), 6);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}

TEST(cardEvent, Attacker1CardWith6InMiddleBATTLEPHASENORMAL){
    using namespace Protocol;
    GameLogic::Player player_count = 6;
    Instance* instance_ptr = nullptr; // not relevant
    GameLogic::Player previous_durak = -1; //no durak 

    Game game(player_count, instance_ptr, previous_durak); // setup the game
    State& s = game.getState(); // fetch state to pass it to the functions

    // array with 12 slots for each cards, bottom slots->attacking, top slots->defending
    // CARDSLOT_1 -> attack
    // CARDSLOT_1_TOP -> defend
    auto& middle  = s.middle_cards; 
    auto& hands   = s.player_hands;
    auto& stage   = s.stage;
    int attacker_idx = findAttacker_TESTHELPER(s);
    int defender_idx = (attacker_idx + 1) % s.player_count;
    int coattacker_idx = (defender_idx + 1) % s.player_count;
    int first_idle = (coattacker_idx + 1) % s.player_count;
    
    //change phase for the actual scenario to happen
    s.battle_type = BATTLETYPE_NORMAL;

    std::cout << "cheking the size of the def hand, its: " << hands[defender_idx].size() << std::endl;
    // hands[defender_idx].erase();


    Card cardmiddle = Card({RANK_TWO, SUIT_SPADES});
    Card cardmiddle2 = Card({RANK_THREE, SUIT_HEARTS});
    Card cardmiddle3 = Card({RANK_THREE, SUIT_CLUBS});
    Card cardmiddle4 = Card({RANK_THREE, SUIT_DIAMONDS});
    Card cardmiddle5 = Card({RANK_THREE, SUIT_SPADES});

    Card card1 = Card({RANK_TWO, SUIT_CLUBS});
    Card card2 = Card({RANK_TWO, SUIT_HEARTS});

    placeCard(attacker_idx, cardmiddle, s);
    placeCard(attacker_idx, cardmiddle2, s);
    placeCard(attacker_idx, cardmiddle3, s);
    placeCard(attacker_idx, cardmiddle4, s);
    placeCard(attacker_idx, cardmiddle5, s);


    bool found1 = true;
    bool found2 = true;
    auto it1 = hands[attacker_idx].find(card1);
    if(it1 == hands[attacker_idx].end()){ //not found
        found1 = false;
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    auto it2 = hands[attacker_idx].find(card2);
    if(it2 == hands[attacker_idx].end()){ //not found
        found2 = false;
    }
    if(found1 && found2){
        //no erase
    }
    if(found1 && !found2){
        //erase the neighbour of 1
        if(it1 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it1));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    if(!found1 && found2){
        //erase the neighbour of 2
        if(it2 == hands[attacker_idx].begin()){
            hands[attacker_idx].erase(*(++it2));
        } else hands[attacker_idx].erase(*hands[attacker_idx].begin());  
    }
    if(!found1 && !found2){
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
        hands[attacker_idx].erase(*hands[attacker_idx].begin());
    }
    // hands[attacker_idx].erase(*hands[attacker_idx].begin()); //erase first card in the set

    hands[attacker_idx].insert(card1);
    hands[attacker_idx].insert(card2);
    std::unordered_set<Card> cards = {card1};
    cardEvent(attacker_idx, cards, s);
    
    // std::cout << "btype: " <<  s.battle_type << " stage: " << s.stage << std::endl;
    EXPECT_EQ(middle[0].has_value(), true);
    EXPECT_EQ(middle[1].has_value(), true);
    EXPECT_EQ(middle[2].has_value(), true);
    EXPECT_EQ(middle[3].has_value(), true);
    EXPECT_EQ(middle[4].has_value(), true);
    EXPECT_EQ(middle[5].has_value(), true);


    EXPECT_EQ(hands[attacker_idx].size(), 5);
    EXPECT_EQ(stage, GAMESTAGE_OPEN);
}
/*TODO:*/