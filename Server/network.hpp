#pragma once

#include "protocol.hpp"
#include <set>

#define ConnectionID unsigned int

typedef std::unique_ptr<Protocol::Message> MessagePtr;

namespace Network {
    void openConnection();
    void sendMessage(std::set<ConnectionID> clients, MessagePtr message);
}