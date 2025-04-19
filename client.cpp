#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

void on_open(client* c, websocketpp::connection_hdl hdl) {
    c->send(hdl, "hello server", websocketpp::frame::opcode::text);
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
    auto con = c.get_connection("ws://localhost:9002", ec);
    if (ec) {
        std::cout << "Connection error: " << ec.message() << std::endl;
        return 1;
    }

    c.connect(con);
    c.run();
}
