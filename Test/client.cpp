#include <iostream>
#include <sockpp/tcp_connector.h>
#include <cassert>
#include <Networking/message.hpp>
#include <stdexcept>
#include <memory>
#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include <string>

int main() {
    std::cout << "TEST MESSAGETYPE_ILLEGAL_MOVE_NOTIFY" << std::endl;
    /*testing illegal notify message*/
    IllegalMoveNotify err_message;
    err_message.error = "Illegal move";
    std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);
    std::string s = em->toJson();

    std::unique_ptr<Message> answer = deserialiseMessage(s);
    IllegalMoveNotify* return_m = dynamic_cast<IllegalMoveNotify*>(answer.get());
    std::cout << "error: " << return_m->error << std::endl;
    
    std::cout << "---------------------------------------------" <<std::endl;
    std::cout << "TEST MESSAGETYPE_CARD_UPDATE" << std::endl;
    // /*testing card update message*/
    // CardUpdate card_message;
    // card_message.opponent_cards[1] =  3; //player with id=1 has 3 cards
    // card_message.opponent_cards[2] = 5; //player 2 has 5 cards
    // card_message.opponent_cards[3] = 7; //player 3 has 7
    // card_message.draw_pile_cards = 5; //5 cards on the pile
    // card_message.trump_card = 10; // trump card = 10
    // card_message.trump_suit = SUIT_HEARTS; //trump suit is spades or whatever 
    // card_message.middle_cards[CARDSLOT_2] = 9; //on slot 2 the card 9
    // card_message.middle_cards[CARDSLOT_6] = 6; //on slot 2 the card 6
    // //hand vector with 5 cards
    // card_message.hand.push_front(21);
    // card_message.hand.push_front(31);
    // card_message.hand.push_front(25);
    // card_message.hand.push_front(24);

    // std::unique_ptr<Message> cardupdatemesg = std::make_unique<CardUpdate>(card_message);
    // std::string scumsg = cardupdatemesg->toJson();

    // std::unique_ptr<Message> answercumsg = deserialiseMessage(scumsg);
    // CardUpdate* return_cumsg = dynamic_cast<CardUpdate*>(answercumsg.get());
    // //es wirds da nöd identisch use cho wil card.toInt nonig implementiert isch
    // std::cout //<< "opponent_cards: " << return_cumsg->opponent_cards    das chammer nöd eif so printe
    //           << "\ndraw_pile_cards: " << return_cumsg->draw_pile_cards
    //           << "\ntrump_card: " << return_cumsg->trump_card.toInt()
    //           << "\ntrump_suit: " << return_cumsg->trump_suit
    //           //<< "\nmiddle_cards: "<< return_cumsg->middle_cards
    //           /*<< "\nhand: " << return_cumsg->hand */<< std::endl;
  
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_PLAYER_UPDATE" << std::endl;
    /*testing player update message*/
    PlayerUpdate player_message;
    player_message.player_names[0] = "Noah";
    player_message.player_names[1] = "Boah";
    player_message.player_count = 2;
    player_message.durak = 0;

    std::unique_ptr<Message> plm = std::make_unique<PlayerUpdate>(player_message);
    std::string pl = plm->toJson();

    std::unique_ptr<Message> solution = deserialiseMessage(pl);

    PlayerUpdate* return_plm = dynamic_cast<PlayerUpdate*>(solution.get());
    std::cout << "player_name id = 0: " << return_plm->player_names[0] 
              << "\nplayer_name id = 1: " << return_plm->player_names[1] 
              << "\nplayer_count = " << return_plm->player_count
              << "\ndurak = " << return_plm->durak << std::endl;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_BATTLE_STATE_UPDATE" << std::endl;
    BattleStateUpdate battle_message;
    //total 5 players, playerid 1 is defending, id 0 and 2 are attacking, id 3 and 4 are idle
    battle_message.defender = 1;
    battle_message.attackers.push_back(0);
    battle_message.attackers.push_back(2);
    battle_message.idle.push_back(3);
    battle_message.idle.push_back(4);

    std::unique_ptr<Message> bam = std::make_unique<BattleStateUpdate>(battle_message);
    std::string battlestr = bam->toJson();

    std::unique_ptr<Message> bamsol = deserialiseMessage(battlestr);

    BattleStateUpdate* return_bam = dynamic_cast<BattleStateUpdate*>(bamsol.get());
    std::cout << "defender id: " << return_bam->defender
              /*<< "\nattacker 1 id: " << return_bam->attackers[0] 
              << "\nattacker 2 id: " << return_bam->attackers[1]
              << "\nidle 1 id: " << return_bam->idle[0] 
              << "\nidle 2 id: " << return_bam->idle[1] */<< std::endl;
    std::cout << "Attackers: "<<std::endl;
    for(auto att : return_bam->attackers){
        std::cout << "id = " << att << std::endl;
    }
    std::cout << "Idle: " <<std::endl;
    for(auto idlepl : return_bam->idle){
        std::cout << "id = " << idlepl << std::endl;
    }
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_AVAILABLE_ACTION_UPDATE" << std::endl;
    /*testing available action update message*/
    AvailableActionUpdate aa_message;
    aa_message.pass_on = true;
    aa_message.ok = false;
    aa_message.pick_up = false;

    std::unique_ptr<Message> aam = std::make_unique<AvailableActionUpdate>(aa_message);
    std::string string_aa = aam->toJson();

    std::unique_ptr<Message> answer_aa = deserialiseMessage(string_aa);
    AvailableActionUpdate* return_aa = dynamic_cast<AvailableActionUpdate*>(answer_aa.get());
    std::cout << "pass on: " << return_aa->pass_on 
              << "\nok: "      << return_aa->ok
              << "\npick up: " << return_aa->pick_up << std::endl;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_GAME_STATE_UPDATE" << std::endl;
    /*testing game state update message*/
    GameStateUpdate gs_message;
    // gs_message.state = GAMESTATE_LOBBY; //state = 0
    gs_message.state = GAMESTATE_DURAK_SCREEN; //state = 4

    std::cout << "testing state: " << gs_message.state <<std::endl;

    std::unique_ptr<Message> gsm = std::make_unique<GameStateUpdate>(gs_message);
    std::string string_gs = gsm->toJson();

    std::unique_ptr<Message> answer_gs = deserialiseMessage(string_gs);
    GameStateUpdate* return_gs = dynamic_cast<GameStateUpdate*>(answer_gs.get());
    std::cout << "returned state: " << return_gs->state<< std::endl;

    std::cout << "---------------------------------------------------" << std::endl;

    TestMessage message;
    message.x = 3;
    message.y = 7;
    message.string = "mhh trash i like trash";
    std::unique_ptr<Message> m = std::make_unique<TestMessage>(message);
    Network::openConnection("localhost", 42069);
    while(true) {
        Network::sendMessage(m);
        std::unique_ptr<Message> awnser = Network::reciveMessage();
        if(awnser == nullptr){
            std::cerr << "This is the issue!" << std::endl;
            return -1;
        }
        else{
            TestMessage* ret = dynamic_cast<TestMessage*>(awnser.get());
            std::cout   << "string: " << ret->string
                        << "\nx: "<< ret->x << std::endl;
        }
    }
}