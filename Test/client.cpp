#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#define DURAK_PROTOCOL_IMPLEMENTATION
#include "protocol.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;

void on_open(client* c, websocketpp::connection_hdl hdl) {
    c->send(hdl, "junk message", websocketpp::frame::opcode::text);

    Protocol::ClientMessageRequestUserData client_message_request_user_data;
    std::list<Protocol::PlayerUUID> p_old = {1312, 321, 54234};
    client_message_request_user_data.players = p_old;
    std::string json = client_message_request_user_data.toJson();
    c->send(hdl, json, websocketpp::frame::opcode::text);

}

void on_message(client* c, websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << "Received: " << msg->get_payload() << std::endl;
}

int main() {
    client c;
    c.init_asio();
    c.set_open_handler(bind(&on_open, &c, std::placeholders::_1));
    c.set_message_handler(bind(&on_message, &c, std::placeholders::_1, std::placeholders::_2));

    websocketpp::lib::error_code ec;
    auto con = c.get_connection("ws://localhost:42069", ec);
    if (ec) {
        std::cout << "Connection error: " << ec.message() << std::endl;
        return 1;
    }

    c.connect(con);
    c.run();
}
