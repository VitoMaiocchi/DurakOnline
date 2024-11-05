#pragma once
#include <Networking/message.hpp>

void broadcastMessage(std::unique_ptr<Message> message);