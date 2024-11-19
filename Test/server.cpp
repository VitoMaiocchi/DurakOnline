#include <iostream>
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <unordered_set>

int main() {

    assert(Card(Card(RANK_QUEEN, SUIT_SPADES).toInt()) == Card(RANK_QUEEN, SUIT_SPADES));

    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);

        std::cout << "RECIVED MESSAGE FROM "<<id<<": \n" << m->toJson() << std::endl; 

        if(m->messageType == MESSAGETYPE_CLIENT_CONNECT_EVENT) {
            GameStateUpdate update;
            update.state = GAMESTATE_GAME;
            Network::sendMessage(std::make_unique<GameStateUpdate>(update), id);

            CardUpdate update2;
            update2.hand = {
                Card(RANK_ACE, SUIT_SPADES),
                Card(RANK_KING, SUIT_DIAMONDS),
                Card(RANK_QUEEN, SUIT_HEARTS),
                Card(RANK_JACK, SUIT_CLUBS)
            };
            update2.middle_cards[CARDSLOT_1] = Card(RANK_ACE, SUIT_SPADES);
            update2.middle_cards[CARDSLOT_3] = Card(RANK_QUEEN, SUIT_SPADES);
            update2.middle_cards[CARDSLOT_5] = Card(RANK_JACK, SUIT_HEARTS);
            update2.middle_cards[CARDSLOT_6] = Card(RANK_KING, SUIT_DIAMONDS);
            update2.middle_cards[CARDSLOT_3_TOP] = Card(RANK_TEN, SUIT_CLUBS);
            update2.middle_cards[CARDSLOT_5_TOP] = Card(RANK_KING, SUIT_HEARTS);

            update2.draw_pile_cards = 1;
            update2.opponent_cards[3] = 3;
            update2.trump_card = Card();
            update2.trump_suit = SUIT_HEARTS;

            Network::sendMessage(std::make_unique<CardUpdate>(update2), id);
        }
    }

    return 0;
}