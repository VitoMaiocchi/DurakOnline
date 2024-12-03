#include <iostream>
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <unordered_set>
#include <csignal>
#include <unistd.h>

std::unordered_set<ClientID> clients;

void cleanup(int signum) {
    std::cout << "\nDisconnecting all clients before closing...\n";
    for(ClientID id : clients) Network::sendMessage(std::make_unique<RemoteDisconnectEvent>(), id);
    sleep(1); //give clients time to disconnect gracefully
    exit(0);
}

int main() {

    signal(SIGINT, cleanup);

    assert(Card(Card(RANK_QUEEN, SUIT_SPADES).toInt()) == Card(RANK_QUEEN, SUIT_SPADES));

    Network::openSocket(42069);
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);

        clients.insert(id);

        std::cout << "RECIVED MESSAGE FROM "<<id<<": \n" << m->toJson() << std::endl; 

        if(m->messageType == MESSAGETYPE_CLIENT_CONNECT_EVENT) {
            GameStateUpdate update;
            update.state = GAMESTATE_LOBBY;
            Network::sendMessage(std::make_unique<GameStateUpdate>(update), id);

            PlayerUpdate update3;
            update3.player_names[8] = "Garbage Goober";
            update3.player_names[7] = "Booger Eater";
            update3.player_names[10] = "Milfhunter";
            update3.player_names[id] = "YOU";

            Network::sendMessage(std::make_unique<PlayerUpdate>(update3), id);

            sleep(2);

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
            update2.opponent_cards[8] = 3;
            update2.opponent_cards[7] = 16;
            update2.opponent_cards[10] = 5;
            update2.trump_card = Card();
            update2.trump_suit = SUIT_HEARTS;

            Network::sendMessage(std::make_unique<CardUpdate>(update2), id);

            BattleStateUpdate update4;
            update4.attackers = {8};
            update4.defender = 7;
            update4.idle = {10};
            Network::sendMessage(std::make_unique<BattleStateUpdate>(update4), id);

            AvailableActionUpdate update5;
            update5.ok = true;
            update5.pass_on = false;
            update5.pick_up = false;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update5), id);

            sleep(5);
            update5.ok = false;
            update5.pass_on = false;
            update5.pick_up = true;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update5), id);


            sleep(5);
            update5.ok = true;
            update5.pass_on = true;
            update5.pick_up = true;
            Network::sendMessage(std::make_unique<AvailableActionUpdate>(update5), id);
        }
    }

    return 0;
}
