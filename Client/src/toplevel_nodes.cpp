#define NETWORKTYPE_CLIENT
#include "toplevel_nodes.hpp"
#include "drawable.hpp"
#include "global_state.hpp"
#include <Networking/network.hpp>
#include <random>
#include <iostream>

// LobbyNode
class Lobby : public LeafNode {
public:
    Lobby() {}

    void updateExtends(Extends new_extends) override {
        extends = new_extends;
    }

    void draw() override {
        // Base rectangle extends
        Extends base_ext = {
            extends.x + extends.width * 0.05f,
            extends.y + extends.height * 0.05f,
            extends.width * 0.9f,
            extends.height * 0.9f,
        };
        OpenGL::drawRectangle(base_ext, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Title extends
        Extends title_ext = {
            extends.x + extends.width * 0.15f,
            extends.y + extends.height * 0.6f,
            extends.width * 0.7f,
            extends.height * 0.3f,
        };
        OpenGL::drawText("LOBBY", title_ext, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_XLARGE);
    }

    Extends getCompactExtends(Extends ext) override {
        return ext;
    }
};


// LobbyNode Implementation
LobbyNode::LobbyNode(Extends ext) {
    lobby = std::make_unique<Lobby>();

    back_button = std::make_unique<ButtonNode>("BACK");
    back_button->setClickEventCallback([](float x, float y){
        std::cout << "back" << std::endl;
    });
    cast(ButtonNode, back_button)->visible = true;

    ready_button = std::make_unique<ButtonNode>("READY");
    ready_button->setClickEventCallback([](float x, float y){
        std::cout << "Ready button pressed..." << std::endl;

        ClientActionEvent event;
        event.action = CLIENTACTION_READY;
        Network::sendMessage(std::make_unique<ClientActionEvent>(event));
    });
    cast(ButtonNode, ready_button)->visible = true;

    settings_button = std::make_unique<ButtonNode>("SETTINGS");
    settings_button->setClickEventCallback([](float x, float y){
        std::cout << "settings" << std::endl;
    });
    cast(ButtonNode, settings_button)->visible = true;

    std::cout << "GlobalState::players.size() = " << GlobalState::players.size() << std::endl;
    for (const auto &player : GlobalState::players) {
        std::cout << "Inside player loop" << std::endl;
        player_nodes.push_back(std::make_unique<PlayerNode>(&player, false));
    }

    updateExtends(ext);
}

void LobbyNode::updateExtends(Extends ext) {
    extends = ext;
    //lobby
    lobby->updateExtends(ext);
    //buttons
    float button_width = ext.width * 0.25f;
    float available_area = ext.width * 0.8f;
    float total_button_width = button_width * 3;
    float spacing = (available_area - total_button_width) / 4;
    float start_x = ext.x + (ext.width - available_area) / 2.0f;
    back_button->updateExtends({
        start_x + spacing,
        ext.y + ext.height * 0.1f,
        button_width,
        ext.height * 0.1f,
    });
    ready_button->updateExtends({
        start_x + spacing * 2 + button_width,
        ext.y + ext.height * 0.1f,
        button_width,
        ext.height * 0.1f,
    });
    settings_button->updateExtends({
        start_x + spacing * 3 + button_width * 2,
        ext.y + ext.height * 0.1f,
        button_width,
        ext.height * 0.1f,
    });

    // Update player nodes layout
    int num_players = static_cast<int>(player_nodes.size());
    if (num_players > 0) {
        Extends player_ext = {
            ext.x + ext.width * 0.1f,
            ext.y + ext.height * 0.3f,
            ext.width * 0.8f,
            ext.height * 0.25f,
        };
        
        float player_width = player_ext.width / num_players;
        for (int i = 0; i < num_players; ++i) {
            Extends current_player_ext = {
                player_ext.x + i * player_width,
                player_ext.y,
                player_width,
                player_ext.height,
            };
            player_nodes[i]->updateExtends(current_player_ext);
        }
    }
}

Extends LobbyNode::getCompactExtends(Extends ext) {
    return ext;
}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(lobby);
    function(back_button);
    function(ready_button);
    function(settings_button);
    for (auto &player_node : player_nodes) {
        function(player_node);
    }
}

//-----------------------------------------------------------------------------------------------------
bool isNumber(const std::string& str) {
    if (str.empty()) return false;
    for (char ch : str) {
        if (!std::isdigit(ch)) return false;
    }
    return true;
}

LoginScreenNode::LoginScreenNode(Extends ext){
    //Text input field for name and for Ip
    name_input = std::make_unique<TextInputNode>("Enter your name");
    name_input->visible = true;
    ip_input = std::make_unique<TextInputNode>("IP Adress");
    ip_input->visible = true;
    OpenGL::setCharacterInputCallback([this](char c) {
        if(ip_input->isFocused()){
            if (ip_input) {
            ip_input->handleCharacterInput(c);
            ip = ip_input->getText();
            }
        }
        if(name_input->isFocused()){
            if (name_input) {
            name_input->handleCharacterInput(c);
            name = name_input->getText();
            }
        }
    });

    //Connect button
    connect_button = std::make_unique<ButtonNode>("CONNECT");
    connect_button->setClickEventCallback([this](float x, float y) {
        std::cout << "Trying to connect to server..." << std::endl;
        if(ip.empty()) return;
        if (!isNumber(ip)) return;
        GlobalState::clientID = Network::openConnection("localhost", std::stoi(ip));
        if(!GlobalState::clientID) {
            //CONNECTION FAILED
            //TODO: (eric) connection error message displaye oder so ka

            std::cout << "Connection failed..." << std::endl;
            return;
        }

        //Set random name if player doesnt choose name
        std::random_device rd;                        
        std::mt19937 gen(rd());                         
        std::uniform_int_distribution<> distr(1, 100000);
        int randomNumber = distr(gen);
        if (name.empty()) name = "Player"+std::to_string(randomNumber);
        //Send message to connect
        ClientConnectEvent event;
        event.username = name;
        Network::sendMessage(std::make_unique<ClientConnectEvent>(event));
    });

    connect_button->visible = true;
    name_input->visible = true;
    ip_input->visible = true;

    updateExtends(ext);
}

void LoginScreenNode::updateExtends(Extends ext){
    extends = ext;
    //button
    connect_button->updateExtends({
        ext.x + ext.width * 0.25f,
        ext.y + ext.height * 0.1f,
        ext.width * 0.5f,
        ext.height * 0.1f,
    });

    ip_input->updateExtends({
        ext.x + ext.width * 0.25f,
        ext.y + ext.height * 0.25f,
        ext.width * 0.5f,
        ext.height * 0.1f,
    });

    name_input->updateExtends({
        ext.x + ext.width * 0.25f,
        ext.y + ext.height * 0.45f,
        ext.width * 0.5f,
        ext.height * 0.1f,
    });
}

void LoginScreenNode::draw() {
    Extends base_ext = {
        extends.x + extends.width * 0.2f,
        extends.y + extends.height * 0.05f,
        extends.width * 0.6f,
        extends.height * 0.9f,
    };
    OpenGL::drawRectangle(base_ext, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Extends title_ext = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.65f,
            extends.width * 0.5f,
            extends.height * 0.3f,
    };
    OpenGL::drawText("DURAK", title_ext, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_XLARGE);
    Extends server_ip_ext = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.45f,
            extends.width * 0.5f,
            extends.height * 0.3f,
    };
    OpenGL::drawText("PLAYER NAME", server_ip_ext, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_LARGE);
    Extends player_name_ext = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.25f,
            extends.width * 0.5f,
            extends.height * 0.3f,
    };
    OpenGL::drawText("SERVER IP: (42069)", player_name_ext, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_LARGE);

    connect_button->draw();
    name_input->draw();
    ip_input->draw();
}

Extends LoginScreenNode::getCompactExtends(Extends ext){
    return ext;
}

void LoginScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node> &)> function) {
    function(connect_button);
    function(name_input);
    function(ip_input);
}

//--------------------------------------------------------------------------------------------------

GameOverScreenNode::GameOverScreenNode(bool durak):durak(durak){
    back_button = std::make_unique<ButtonNode>("BACK");
    back_button->setClickEventCallback([](float x, float y){
        std::cout << "back" << std::endl;
    });
    cast(ButtonNode, back_button)->visible = true;

    rematch_button = std::make_unique<ButtonNode>("REMATCH");
    rematch_button->setClickEventCallback([](float x, float y){
        std::cout << "rematch" << std::endl;
    });
    cast(ButtonNode, rematch_button)->visible = true;

}

void GameOverScreenNode::updateExtends(Extends ext){
    extends = ext;
    //buttons
    float button_width = ext.width * 0.25f;
    float available_area = ext.width * 0.8f;
    float total_button_width = button_width * 2;
    float spacing = (available_area - total_button_width) / 3;
    float start_x = ext.x + (ext.width - available_area) / 2.0f;
    back_button->updateExtends({
        start_x + spacing,
        ext.y + ext.height * 0.1f,
        button_width,
        ext.height * 0.1f,
    });
    rematch_button->updateExtends({
        start_x + spacing * 2 + button_width,
        ext.y + ext.height * 0.1f,
        button_width,
        ext.height * 0.1f,
    });
}

Extends GameOverScreenNode::getCompactExtends(Extends ext){
    return ext;
}

void GameOverScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function){
    //Base
    function(back_button);
    function(rematch_button);
}

void GameOverScreenNode::draw() {
    Extends base_ext = {
        extends.x + extends.width * 0.15f,
        extends.y + extends.height * 0.05f,
        extends.width * 0.7f,
        extends.height * 0.9f,
    };
    OpenGL::drawRectangle(base_ext, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    back_button->draw();
    rematch_button->draw();
}
