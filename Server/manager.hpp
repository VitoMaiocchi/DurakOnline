#pragma once

#include "network.hpp"

namespace Manager {
    void handleMessage(ConnectionID client, MessagePtr message);
    void handleConnect(ConnectionID client);
    void handleDisconnect(ConnectionID client);
}