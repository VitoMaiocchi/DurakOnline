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
    /*testing illegal notify message*/
    IllegalMoveNotify err_message;
    err_message.error = "Illegal move";
    std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);
    std::string s = em->toJson();

    std::unique_ptr<Message> answer = deserialiseMessage(s);
    IllegalMoveNotify* return_m = dynamic_cast<IllegalMoveNotify*>(answer.get());
    std::cout << "error: " << return_m->error << std::endl;
     

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
  

    /*testing player update message*/
    // PlayerUpdate player_message;
    // player_message.player_names[0] = "Noah";
    // player_message.player_names[1] = "Boah";
    // player_message.player_count = 2;
    // player_message.durak = 0;

    // std::unique_ptr<Message> plm = std::make_unique<PlayerUpdate>(player_message);
    // std::string pl = plm->toJson();

    // std::unique_ptr<Message> solution = deserialiseMessage(pl);
    // PlayerUpdate* return_plm = dynamic_cast<PlayerUpdate*>(solution.get());
    // std::cout << "player_name id = 0: " << return_plm->player_names[0] 
    //           << "\nplayer_name id = 1: " << return_plm->player_names[1]
    //           << "\nplayer_count = " << return_plm->player_count
    //           << "\ndurak = " << return_plm->durak << std::endl;

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