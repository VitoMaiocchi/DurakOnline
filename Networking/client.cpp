#include <iostream>
#include <sockpp/tcp_connector.h>
#include <cassert>
#include "message.hpp"
#include <stdexcept>
#include <memory>

int main() {

    //JSON TEST
    TestMessage message;
    message.string = "googoogaga";
    message.x = 1;
    message.y = 69;
    std::string s = message.toJson();
    std::cout << s << std::endl;
    std::unique_ptr<Message> kys = deserialiseMessage(s);
    TestMessage* idk = dynamic_cast<TestMessage*>(kys.get());
    std::cout << idk->string << std::endl;

    //NETWORKING TEST
    std::cout << "Client started" << std::endl;

    sockpp::tcp_connector connector;
    assert( connector.connect(sockpp::inet_address("127.0.0.1", 42069)) );
    connector.send(s);

    // Receive response
    char buffer[1024];
    size_t n = connector.recv(buffer, sizeof(buffer)).value_or_throw();

    if (n > 0) {
        std::cout << "Received from server: " << std::string(buffer, n) << std::endl;
    }
    
}