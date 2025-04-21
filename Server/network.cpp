#define DURAK_PROTOCOL_IMPLEMENTATION
#include "network.hpp"
#include "manager.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>


typedef websocketpp::server<websocketpp::config::asio> server;

namespace Network {
    void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
        auto message = Protocol::deserialiseMessage(msg->get_payload()); 
        if(message) {
            Manager::handleMessage(hdl, std::move(message));
        } else {
            s->send(hdl, "invalid message - not following protocol", msg->get_opcode());
        }
    }

    void on_open(server* s, websocketpp::connection_hdl hdl) {
        Manager::handleConnect(hdl);
    }

    void on_close(server* s, websocketpp::connection_hdl  hdl) {
        Manager::handleDisconnect(hdl);
    }

    server durak_server;

    void openConnection() {
        durak_server.set_message_handler(bind(&on_message, &durak_server, std::placeholders::_1, std::placeholders::_2));
        durak_server.set_open_handler(bind(&on_open, &durak_server, std::placeholders::_1));
        durak_server.set_close_handler(bind(&on_close, &durak_server, std::placeholders::_1));

        durak_server.init_asio();
        durak_server.listen(42069);
        durak_server.start_accept();

        auto& io_context = durak_server.get_io_context();
        while (true) {
            io_context.poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void sendMessage(std::list<ConnectionID> clients, MessagePtr message) {
        std::string m = message->toJson();
        for(ConnectionID id : clients) durak_server.send(id, m, websocketpp::frame::opcode::text);
    }
}