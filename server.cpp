#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    s->send(hdl, "echo: " + msg->get_payload(), msg->get_opcode());
}

int main() {
    server echo_server;
    echo_server.set_message_handler(bind(&on_message, &echo_server, std::placeholders::_1, std::placeholders::_2));
    echo_server.init_asio();
    echo_server.listen(9002);
    echo_server.start_accept();
    echo_server.run();
}
