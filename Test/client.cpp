#include <iostream>
#include <sockpp/tcp_connector.h>
#include <cassert>
#include <Networking/message.hpp>
#include <stdexcept>
#include <memory>
#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>

int main() {

    IllegalMoveNotify err_message;
    err_message.error = "Illegal move";
    std::unique_ptr<Message> em = std::make_unique<IllegalMoveNotify>(err_message);


    TestMessage message;
    message.x = 3;
    message.y = 7;
    message.string = "mhh trash i like trash";
    std::unique_ptr<Message> m = std::make_unique<TestMessage>(message);
    Network::openConnection("localhost", 42069);
    while(true) {
        Network::sendMessage(m);
        std::unique_ptr<Message> awnser = Network::reciveMessage();
        TestMessage* ret = dynamic_cast<TestMessage*>(awnser.get());
        std::cout   << "string: " << ret->string
                    << "\nx: "<< ret->x << std::endl;
        Network::sendMessage(em);
        std::unique_ptr<Message> answer = Network::reciveMessage();
        IlligalMoveNotify* return_m = dynamic_cast<IllegalMoveNotify*>(answer.get());
        std::cout << "error: " << return_m->error << std::endl;
    }
}