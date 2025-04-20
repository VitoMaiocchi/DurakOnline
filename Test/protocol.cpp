#define DURAK_PROTOCOL_IMPLEMENTATION
#include "protocol.hpp"

using namespace Protocol;

int main() {

    std::cout << "RUNNING PROTOCOL SERIALIZATION TEST...\n\n\n";
    
    std::cout << "[ClientMessage RequestUserData]" << std::endl;
    ClientMessageRequestUserData client_message_request_user_data;
    std::list<PlayerUUID> p_old = {1312, 321, 54234};
    client_message_request_user_data.players = p_old;
    std::string json = client_message_request_user_data.toJson();
    std::cout << "json : " << json << std::endl;
    auto message = deserialiseMessage(json);
    auto p_new = dynamic_cast<ClientMessageRequestUserData*>(message.get())->players;
    if (p_old == p_new) std::cout << "   ->PASS" << std::endl;
    else std::cout << "   ->ERROR" << std::endl;
}