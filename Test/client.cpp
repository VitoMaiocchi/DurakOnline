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
void sendClientConnectMessage(ClientID client_id);
void sendReadyMessage(ClientID client_id);
void sendPickUpMessage(ClientID client_id);
void sendPlayCardEvent(ClientID client_id);

void receiveMessages();

ClientID client_id;

int main() {
//the other tests for the messages are in client_messages.txt
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
            // std::cout << "1. Send Test Message\n";
            std::cout << "1. Send Connect Message\n";
            std::cout << "2. Send Ready Message\n";
            std::cout << "3. Send Play Card Event\n";
            std::cout << "4. Send Play Card Event\n";

            std::cout << "5. Exit\n";
            std::cout << "Enter your choice: ";
            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1:
                    // sendTestMessage(client_id);
                    sendClientConnectMessage(client_id);
                    break;
                case 2:
                    sendReadyMessage(client_id);
                    break;
                case 3:
                    sendPlayCardEvent(client_id);
                    break;
                case 4:
                    sendPickUpMessage(client_id);
                    break;
                case 5:
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

//function to send a client connect message
void sendClientConnectMessage(ClientID client_id){
    ClientConnectEvent client;
    std::cout << "Enter your username"<<std::endl;
    std::cin >> client.username;
    Network::sendMessage(std::make_unique<ClientConnectEvent>(client));
    std::cout << "Sent connect message to server." << std::endl;
}


// Function to send a ready message
void sendReadyMessage(ClientID client_id) {
    ClientActionEvent action;
    action.action = CLIENTACTION_READY;

    Network::sendMessage(std::make_unique<ClientActionEvent>(action));
    std::cout << "Sent ready message to server." << std::endl;
}

void sendPickUpMessage(ClientID client_id){

}

// Function to send a play card event
void sendPlayCardEvent(ClientID client_id) {
    PlayCardEvent card_event;
    Card card1(RANK_QUEEN, SUIT_SPADES);
    // Card card2(RANK_KING, SUIT_HEARTS);

    card_event.cards.insert(card1);
    // card_event.cards.insert(card2);
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
                    BattleStateUpdate* bsu_msg = dynamic_cast<BattleStateUpdate*>(msg.get());
                    std::cout << "\nReceived Battle State Update Message:" << std::endl;

                    if(bsu_msg->attackers.front() == client_id){
                        std::cout << "You are the attacker!" << std::endl;
                    }
                    else if(bsu_msg->attackers.back() == client_id){
                        std::cout << "You are the co-attacker!" << std::endl;
                    }
                    else if(bsu_msg->defender == client_id){
                        std::cout << "You are the defender!" << std::endl;
                    }
                    else{
                        std::cout << "You are only observer in this battle" << std::endl;
                    }

                    
                    break;
                }
                case MESSAGETYPE_AVAILABLE_ACTION_UPDATE: {
                    std::cout << "\nReceived Available Action Update Message." << std::endl;
                    break;
                }
                case MESSAGETYPE_GAME_STATE_UPDATE: {
                    std::cout << "\nReceived Game State Update Message:" << std::endl;
                    GameStateUpdate* update_msg = dynamic_cast<GameStateUpdate*>(msg.get());
                    std::cout << "state: " << update_msg->state << "\n" << std::endl;

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