#include "manager.hpp"

namespace Manager {
    void handleMessage(ConnectionID client, MessagePtr message) {
        //echo for test
        Network::sendMessage({client}, std::move(message));
    }

    void handleConnect(ConnectionID client) {

    }

    void handleDisconnect(ConnectionID client) {

    }
}

int main() {
    Network::openConnection();
}