#include <iostream>
#include <sockpp/tcp_connector.h>
#include <cassert>
#include "message.hpp"
#include <stdexcept>
#include <memory>
#define NETWORKTYPE_CLIENT
#include "network.hpp"

int main() {
    TestMessage message;
    message.x = 3;
    message.y = 7;
    message.string = "mhh trash i like trash";
    std::unique_ptr<Message> m = std::make_unique<TestMessage>(message);
    Network::openConnection("localhost", 42069);
    while(true) {
        //sleep(1);
        Network::sendMessage(m);
        std::unique_ptr<Message> awnser = Network::reciveMessage();
        TestMessage* ret = dynamic_cast<TestMessage*>(awnser.get());
        std::cout   << "string: " << ret->string
                    << "\n x: "<< ret->x << std::endl;
    }
}