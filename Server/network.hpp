#pragma once

#include "protocol.hpp"
#include <set>

typedef std::unique_ptr<Protocol::Message> MessagePtr;
typedef std::weak_ptr<void> ConnectionID;

namespace Network {
    void openConnection();
    void sendMessage(std::list<ConnectionID> clients, MessagePtr message);
}