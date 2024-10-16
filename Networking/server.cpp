#include <iostream>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_socket.h>
#include <cassert>
#include "message.hpp"

int main() {
    //NETWORKING TEST
    std::cout << "Server started" << std::endl;

    sockpp::tcp_acceptor acceptor(42069);
    assert(acceptor);

    std::cout << "Waiting for a connection..." << std::endl;
    sockpp::socket socket = acceptor.accept().release_or_throw();
    std::cout << "Client connected!" << std::endl;

    // Receive data
    char buffer[1024];
    size_t n = socket.recv(buffer, sizeof(buffer)).value_or_throw();
    if (n>0) {
        auto s = std::string(buffer, n);
        std::cout << "Received: " << s << std::endl;
        std::unique_ptr<Message> kys = deserialiseMessage(s);
        TestMessage* idk = dynamic_cast<TestMessage*>(kys.get());
        std::cout << idk->string << std::endl;
        std::cout << idk->x << std::endl;
        std::cout << idk->y << std::endl;
        // Echo
        socket.send(std::string(buffer, n));
    }

    return 0;
}