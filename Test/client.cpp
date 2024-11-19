#include <iostream>
#include <sockpp/tcp_connector.h>
#include <cassert>
#include <Networking/message.hpp>
#include <stdexcept>
#include <memory>
#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include <string>
#include <thread>
#include <chrono>


// Function prototypes
void sendTestMessage(ClientID client_id);
void sendReadyMessage(ClientID client_id);
void sendPlayCardEvent(ClientID client_id);
void receiveMessages();

ClientID client_id;

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

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "---------------------------------------------" <<std::endl;
    std::cout << "TEST MESSAGETYPE_CARD_UPDATE" << std::endl;
    // /*testing card update message*/

    // std::map<ClientID, unsigned int> opponent_cards; //Map ClientID to card count 
    // unsigned int draw_pile_cards; 
    // Card trump_card; //the one that is on the bottom of the pile, can also be NULL
    // Suit trump_suit;
    // std::map<CardSlot, Card> middle_cards; //map of slot to card
    // std::list<Card> hand; //list of cards in hand

    CardUpdate card_message;
    card_message.opponent_cards[1] =  3; //player with id=1 has 3 cards
    card_message.opponent_cards[2] = 5; //player 2 has 5 cards
    card_message.opponent_cards[3] = 7; //player 3 has 7
    card_message.draw_pile_cards = 5; //5 cards on the pile
    Card trump_card_ = Card(RANK_TEN, SUIT_HEARTS);
    card_message.trump_card = trump_card_; // trump card = 10 of hearts
    card_message.trump_suit = SUIT_HEARTS; //trump suit is spades or whatever
    Card nine_spades = Card(RANK_NINE, SUIT_SPADES);
    Card nine_clubs =  Card(RANK_NINE, SUIT_CLUBS);
    card_message.middle_cards[CARDSLOT_1] = nine_spades; //on slot 1 the card nine of spades
    card_message.middle_cards[CARDSLOT_2] = nine_clubs; //on slot 2 the card nine of clubs
    //hand vector with 5 cards
    card_message.hand.push_front(21);
    card_message.hand.push_front(31);
    card_message.hand.push_front(25);
    card_message.hand.push_front(24);

    std::unique_ptr<Message> cardupdatemesg = std::make_unique<CardUpdate>(card_message);
    std::string scumsg = cardupdatemesg->toJson();

try {
        std::unique_ptr<Message> answercumsg = deserialiseMessage(scumsg);
        CardUpdate* return_cumsg = dynamic_cast<CardUpdate*>(answercumsg.get());
        // Further processing


    std::cout << "Opponent Cards:\t" << "ID\t" << "Card amount\n"; 
    for(auto& opps : return_cumsg->opponent_cards){
        std::cout << "opp:\t\t" << opps.first<<"\t\t" << opps.second <<"\n";
    }

    std::cout << "\ndraw_pile_cards: \t" << return_cumsg->draw_pile_cards
              << "\ntrump_card: \t" << return_cumsg->trump_card.rank << "-"<< return_cumsg->trump_card.suit
              << "\ntrump_suit: \t" << return_cumsg->trump_suit <<std::endl;

    std::cout << "Middle Cards:\t" << "Slot\t" << "Card\n"; 
    for(auto& cards : return_cumsg->middle_cards){
        std::cout << "slot:\t\t" << cards.first <<"\t" << cards.second.rank << "-"<< cards.second.suit<<"\n"; 
    }

    std::cout << "My hand:\t";
    for(auto& card : return_cumsg->hand){
        std::cout << "\t" << card.rank << "-" << card.suit << " ";
    }

} catch (const std::exception& e) {
        std::cerr << "Error during deserialization: " << e.what() << std::endl;
}
    std::cout << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
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
    // std::unique_ptr<Message> bamsol = Network::reciveMessage(); //added a recieve msg 
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
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_AVAILABLE_ACTION_UPDATE" << std::endl;
    /*testing available action update message*/
    AvailableActionUpdate aa_message;
    aa_message.pass_on = false;
    aa_message.ok = true;
    aa_message.pick_up = true;

    std::unique_ptr<Message> aam = std::make_unique<AvailableActionUpdate>(aa_message);
    std::string string_aa = aam->toJson();
    std::unique_ptr<Message> answer_aa = deserialiseMessage(string_aa);

    AvailableActionUpdate* return_aa = dynamic_cast<AvailableActionUpdate*>(answer_aa.get());
    std::cout << "pass on: " << return_aa->pass_on 
                << "\nok: "      << return_aa->ok
                << "\npick up: " << return_aa->pick_up << std::endl;
    

    std::cout << "---------------------------------------------------" << std::endl;
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
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_PLAY_CARD_EVENT" << std::endl;
    /*testing play card event message*/
    PlayCardEvent pce_message;
    Card piqueQ;
    piqueQ.suit = SUIT_SPADES;
    piqueQ.rank = RANK_QUEEN;
    Card heartK(RANK_KING, SUIT_HEARTS);
    pce_message.cards.insert(piqueQ); 
    pce_message.cards.insert(heartK);

    pce_message.slot = CARDSLOT_2_TOP;

    std::unique_ptr<Message> pcem = std::make_unique<PlayCardEvent>(pce_message);
    std::string string_pce = pcem->toJson();

    std::unique_ptr<Message> answer_pce = deserialiseMessage(string_pce);
    PlayCardEvent* return_pce = dynamic_cast<PlayCardEvent*>(answer_pce.get());
    // std::cout << "username: " << return_pce->username<< std::endl;

    std::cout << "Cards: "<<std::endl;
    for(auto card : return_pce->cards){
        std::cout << "card suit = " << card.suit 
                  << "\ncard rank = " << card.rank << std::endl;
    }
    std::cout << "card slot: " << return_pce->slot <<std::endl;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TEST MESSAGETYPE_CLIENT_ACTION_EVENT" << std::endl;
    /*testing client action event message*/
    ClientActionEvent cae_message;
    cae_message.action = CLIENTACTION_PASS_ON; //action = 1

    std::cout << "testing action: " << cae_message.action <<std::endl;

    std::unique_ptr<Message> caem = std::make_unique<ClientActionEvent>(cae_message);
    std::string string_cae = caem->toJson();

    std::unique_ptr<Message> answer_cae = deserialiseMessage(string_cae);
    ClientActionEvent* return_cae = dynamic_cast<ClientActionEvent*>(answer_cae.get());
    std::cout << "returned action: " << return_cae->action<< std::endl;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    std::cout << "TEST MESSAGETYPE_CLIENT_CONNECT_EVENT" << std::endl;
    /*testing client action event message*/
    ClientConnectEvent cce_message;
    cce_message.username = "Ericberic"; //

    // std::cout << "testing action: " << cae_message.action <<std::endl;

    std::unique_ptr<Message> ccem = std::make_unique<ClientConnectEvent>(cce_message);
    std::string string_cce = ccem->toJson();

    std::unique_ptr<Message> answer_cce = deserialiseMessage(string_cce);
    ClientConnectEvent* return_cce = dynamic_cast<ClientConnectEvent*>(answer_cce.get());
    std::cout << "username: " << return_cce->username<< std::endl;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    // std::cout << "TEST MESSAGETYPE_TEST and sending a client card update msg"<<std::endl;


    // TestMessage message;
    // message.x = 3;
    // message.y = 7;
    // message.string = "mhh trash i like trash";
    // std::unique_ptr<Message> m = std::make_unique<TestMessage>(message);
    // Network::openConnection("localhost", 42069);
    // Network::sendMessage(m);
    // while(true) {

    //     std::unique_ptr<Message> awnser = nullptr;
    //     while(!awnser) awnser = Network::reciveMessage();
    //     TestMessage* ret = dynamic_cast<TestMessage*>(awnser.get());
    //     std::cout   << "string: " << ret->string
    //                 << "\nx: "<< ret->x  
    //                 << "\ny: " << ret->y << std::endl;

    // }
try {
        // Connect to the server
        std::cout << "Connecting to server..." << std::endl;
        client_id = Network::openConnection("127.0.0.1", 42069);
        std::cout << "Connected to server with client ID: " << client_id << std::endl;

        // Spawn a thread to handle incoming messages
        std::thread receiver_thread(receiveMessages);

        // Main loop to send test messages to the server
        while (true) {
            std::cout << "\nChoose an action to send to the server:\n";
            std::cout << "1. Send Test Message\n";
            std::cout << "2. Send Ready Message\n";
            std::cout << "3. Send Play Card Event\n";
            std::cout << "4. Exit\n";
            std::cout << "Enter your choice: ";
            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1:
                    sendTestMessage(client_id);
                    break;
                case 2:
                    sendReadyMessage(client_id);
                    break;
                case 3:
                    sendPlayCardEvent(client_id);
                    break;
                case 4:
                    std::cout << "Exiting client." << std::endl;
                    receiver_thread.detach(); // Let the thread clean itself up
                    return 0;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}

// Function to send a test message
void sendTestMessage(ClientID client_id) {
    TestMessage message;
    message.string = "This is a test message";
    message.x = 42;
    message.y = 24;

    Network::sendMessage(std::make_unique<TestMessage>(message));
    std::cout << "Sent test message to server." << std::endl;
}

// Function to send a ready message
void sendReadyMessage(ClientID client_id) {
    ClientActionEvent action;
    action.action = CLIENTACTION_READY;

    Network::sendMessage(std::make_unique<ClientActionEvent>(action));
    std::cout << "Sent ready message to server." << std::endl;
}

// Function to send a play card event
void sendPlayCardEvent(ClientID client_id) {
    PlayCardEvent card_event;
    Card card1(RANK_QUEEN, SUIT_SPADES);
    Card card2(RANK_KING, SUIT_HEARTS);

    card_event.cards.insert(card1);
    card_event.cards.insert(card2);
    card_event.slot = CARDSLOT_2_TOP;

    Network::sendMessage(std::make_unique<PlayCardEvent>(card_event));
    std::cout << "Sent play card event to server." << std::endl;
}

// Thread function to receive messages from the server
void receiveMessages() {
    try {
        while (true) {
            std::unique_ptr<Message> msg = Network::reciveMessage();
            if (!msg) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep briefly to avoid busy-waiting
                continue;
            }

            // Print received message details
            switch (msg->messageType) {
                case MESSAGETYPE_TEST: {
                    TestMessage* test_msg = dynamic_cast<TestMessage*>(msg.get());
                    std::cout << "\nReceived Test Message: " << test_msg->string
                              << " | x: " << test_msg->x << " | y: " << test_msg->y << std::endl;
                    break;
                }
                case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY: {
                    IllegalMoveNotify* err_msg = dynamic_cast<IllegalMoveNotify*>(msg.get());
                    std::cout << "\nReceived Illegal Move Notify: " << err_msg->error << std::endl;
                    break;
                }
                case MESSAGETYPE_CARD_UPDATE: {
                    std::cout << "\nReceived Card Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_PLAYER_UPDATE: {
                    std::cout << "\nReceived Player Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_BATTLE_STATE_UPDATE: {
                    std::cout << "\nReceived Battle State Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_AVAILABLE_ACTION_UPDATE: {
                    std::cout << "\nReceived Available Action Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_GAME_STATE_UPDATE: {
                    std::cout << "\nReceived Game State Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_PLAYCARD_EVENT: {
                    std::cout << "\nReceived Play Card Event Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_CLIENT_ACTION_EVENT: {
                    std::cout << "\nReceived Client Action Event Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_CLIENT_CONNECT_EVENT: {
                    std::cout << "\nReceived Client Connect Event Message." << std::endl;
                    break;
                }
                default:
                    std::cout << "\nUnknown message type received from server." << std::endl;
                    break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Receiver Thread Exception: " << e.what() << std::endl;
    }

}