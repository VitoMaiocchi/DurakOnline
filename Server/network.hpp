#pragma once

#include "protocol.hpp"

#define ConnectionID unsigned int

namespace Network {
    void sendMessage(std::set<ConnectionID> clients, MessagePtr message);
}