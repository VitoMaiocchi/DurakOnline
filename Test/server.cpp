#include <iostream>
#include <Networking/message.hpp>
#define NETWORKTYPE_SERVER
#include <Networking/network.hpp>
#include <unordered_set>

int main() {
    Network::openSocket(42069);
    std::unordered_set<ClientID> clients;
    while(true) {
        ClientID id;
        std::unique_ptr<Message> m = Network::reciveMessage(id);
        if(m == nullptr){
            std::cerr << "null ptr received" <<std::endl;
            return -1;
        }
        else{
            std::cout << "message received" << std::endl;
        }
        clients.insert(id);

        switch (m->messageType) {
            case MESSAGETYPE_TEST:
                dynamic_cast<TestMessage*>(m.get())->x = id;
                for(auto client : clients) {
                    Network::sendMessage(m, client);
                    std::cout << "message sending with type: " << m->messageType << std::endl;
                }
            break;
            case MESSAGETYPE_AVAILABLE_ACTION_UPDATE: {
                AvailableActionUpdate aa_message;
                aa_message.pass_on = true;
                aa_message.ok = false;
                aa_message.pick_up = false;

                std::unique_ptr<Message> aam = std::make_unique<AvailableActionUpdate>(aa_message);
                for(auto client : clients){
                    Network::sendMessage(aam,client);
                    std::cout <<"message sending with type: " << aam->messageType << std::endl;
                }
            }
            break;
            case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
                std::cout << "CLIENT DISCONNECTED: "<<id << std::endl;
            break;
        }
    }

    // if(clients.size() > 0){
    //     BattleStateUpdate bsu_msg;
    //     for(auto pl : clients){
    //         if(pl == 1){
    //             bsu_msg.attackers.push_back(pl);
    //         }
    //         else if(pl == 2){
    //             bsu_msg.defender = pl;
    //         }
    //         else if(pl == 3){
    //             bsu_msg.attackers.push_back(pl);
    //         }
    //         else if(pl > 3){
    //             bsu_msg.idle.push_back(pl);
    //         }
    //     }
    //     std::unique_ptr<Message> bsu = std::make_unique<BattleStateUpdate>(bsu_msg);
    //     for(auto pl : clients){
    //         Network::sendMessage(bsu, pl); //maybe make function to broadcast to all
    //     }
    // }


    return 0;
}