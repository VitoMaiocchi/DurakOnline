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
        OpenGL::drawText("LOBBY", title_ext, COLOR_BLACK, TEXTSIZE_XLARGE);
    }

    Extends getCompactExtends(Extends ext) override {
        return ext;
    }
};

//Settings
class Settings_screen : public Node {
    bool show = false;
public:
    std::unique_ptr<Node> back_button;

    Settings_screen() : show(false) {
        back_button = std::make_unique<ButtonNode>("BACK");
        back_button->setClickEventCallback([this](float x, float y){
            std::cout << "BACK (from Setting screen)" << std::endl;
            updateShow(false);
        });
        cast(ButtonNode, back_button)->visible = this->show;
    }

    void updateShow(bool show) {
        this->show = show;
        cast(ButtonNode, back_button)->visible = this->show;
    }

    void updateExtends(Extends ext) override {
        extends = ext;

        back_button->updateExtends({
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.1f,
            extends.width * 0.5f,
            extends.height * 0.1f,
        });
    }

    void draw() override {
        if(!show) return;
        //Background
        OpenGL::drawRectangle(extends, glm::vec4(.5f, .5f, .5f, 1.0f));
        // Base rectangle extends
        Extends base_ext = {
            extends.x + extends.width * 0.2f,
            extends.y + extends.height * 0.05f,
            extends.width * 0.6f,
            extends.height * 0.9f,
        };
        OpenGL::drawRectangle(base_ext, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Title extends
        Extends title_ext = {
                extends.x + extends.width * 0.25f,
                extends.y + extends.height * 0.65f,
                extends.width * 0.5f,
                extends.height * 0.3f,
        };
        OpenGL::drawText("SETTINGS", title_ext, COLOR_BLACK, TEXTSIZE_XLARGE);

        Extends sort = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.45f,
            extends.width * 0.5f,
            extends.height * 0.3f,
        };
        OpenGL::drawText("SORT BY", sort, COLOR_BLACK, TEXTSIZE_LARGE);

        back_button->draw();
    }

    Extends getCompactExtends(Extends ext) override {
        return ext;
    }

    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) override {
        function(back_button);
    }

};

// LobbyNode Implementation
LobbyNode::LobbyNode(Extends ext) {
    lobby = std::make_unique<Lobby>();
    setting = std::make_unique<Settings_screen>();

    back_button = std::make_unique<ButtonNode>("BACK");
    back_button->setClickEventCallback([](float x, float y){
        std::cout << "back" << std::endl;
        Network::closeConnection();
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
    cast(ButtonNode, settings_button)->visible = true;
    settings_button->setClickEventCallback([this](float x, float y){
        std::cout << "settings" << std::endl;

        cast(Settings_screen, this->setting)->updateShow(true);
        cast(ButtonNode, back_button)->visible = false;
        cast(ButtonNode, ready_button)->visible = false;
        cast(ButtonNode, settings_button)->visible = false;
    });

    playerUpdateNotify();

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

    Extends player_ext = {
        ext.x + ext.width * 0.1f,
        ext.y + ext.height * 0.3f,
        ext.width * 0.8f,
        ext.height * 0.25f,
    };

    int num_players = static_cast<int>(player_nodes.size());
    float player_width = player_ext.width / num_players;

    int i = 0; // Counter for player index
    for (auto& node : player_nodes) { // Use a range-based loop or iterator
        Extends current_player_ext = {
            player_ext.x + i * player_width,
            player_ext.y,
            player_width,
            player_ext.height,
        };
        node->updateExtends(current_player_ext); // Update the current player's extends
        ++i;
    }

    setting->updateExtends(ext);
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
    function(setting);
}

void LobbyNode::playerUpdateNotify() {
    player_nodes.clear();
    if(GlobalState::players.size() == 0) return;

    for(auto &player : GlobalState::players) player_nodes.push_back(std::make_unique<PlayerNode>(&player, false));

    updateExtends(extends);
}

void LobbyNode::handleReadyUpdate(ReadyUpdate update) {
    for(const Player &player : GlobalState::players) {
        if(update.players.find(player.id) == update.players.end()) player.lobby->ready = false;
        else player.lobby->ready = true;
    }
}

void LobbyNode::draw() {
    lobby->draw();
    back_button->draw();
    if(!GlobalState::players.find({GlobalState::clientID})->lobby->ready) ready_button->draw();
    settings_button->draw();
    for (auto &player_node : player_nodes) {
        player_node->draw();
    }
    setting->draw();
}

//-----------------------------------------------------------------------------------------------------

void LoginScreenNode::connect() {
    std::cout << "Trying to connect to server..." << std::endl;
    if(ip.empty()) ip = "localhost";
    GlobalState::clientID = Network::openConnection(ip, 42069);
    if(!GlobalState::clientID) {
        //CONNECTION FAILED
        Viewport::createPopup("Connection Failed...", 3);
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
}

LoginScreenNode::LoginScreenNode(Extends ext){
    //Text input field for name and for Ip
    name_input = std::make_unique<TextInputNode>("Enter your name");
    name_input->visible = true;
    ip_input = std::make_unique<TextInputNode>("localhost");
    ip_input->visible = true;
    OpenGL::setCharacterInputCallback([this](char c) {
        if(c == '\n') {
            connect();
            return;
        }
        if(cast(TextInputNode, ip_input)->isFocused()){
            if (ip_input) {
                cast(TextInputNode, ip_input)->handleCharacterInput(c);
                ip = cast(TextInputNode, ip_input)->getText();
            }
        }
        if(cast(TextInputNode, name_input)->isFocused()){
            if (name_input) {
                if(name.size() > 30 && c != '\b'){
                    Viewport::createPopup("Name to long", 3);
                }else{
                    cast(TextInputNode, name_input)->handleCharacterInput(c);
                    name = cast(TextInputNode, name_input)->getText();
                }
            }
        }
    });

    //Connect button
    connect_button = std::make_unique<ButtonNode>("CONNECT");
    connect_button->setClickEventCallback([this](float x, float y) {
        connect();
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
            extends.x + extends.width * 0.175f,
            extends.y + extends.height * 0.65f,
            extends.width * 0.65f,
            extends.height * 0.3f,
    };
    auto size = OpenGL::getImageDimensions(CLIENT_RES_DIR+"fonts/durak.png");
    OpenGL::drawImage(CLIENT_RES_DIR+"fonts/durak.png", computeCompactExtends(title_ext, size.second, size.first));
    Extends server_ip_ext = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.45f,
            extends.width * 0.5f,
            extends.height * 0.3f,
    };
    OpenGL::drawText("PLAYER NAME", server_ip_ext, COLOR_BLACK, TEXTSIZE_LARGE);
    Extends player_name_ext = {
            extends.x + extends.width * 0.25f,
            extends.y + extends.height * 0.25f,
            extends.width * 0.5f,
            extends.height * 0.3f,
    };
    OpenGL::drawText("HOSTNAME / IP", player_name_ext, COLOR_BLACK, TEXTSIZE_LARGE);

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

GameOverScreenNode::GameOverScreenNode(Extends ext, bool durak):durak(durak){
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
    updateExtends(ext);
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


