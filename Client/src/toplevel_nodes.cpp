#define NETWORKTYPE_CLIENT
#include "toplevel_nodes.hpp"
#include "drawable.hpp"
#include "master_node.hpp"
#include <Networking/network.hpp>

// LobbyNode
class Lobby : public LeafNode {
private:

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

        // Player extends
        Extends player_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.3f,
            extends.width * 0.8f,
            extends.height * 0.25f,
        };
        drawPlayers(player_ext);
    }

    void drawPlayers(const Extends& player_ext) {
        int num_players = GlobalState::players.size();
        if (num_players == 0) return;
        float player_width = player_ext.width / num_players;

        int index = 0;
        for (const auto& player : GlobalState::players) {
            // Calculate the extends for the current player
            Extends current_player_ext = {
                player_ext.x + index * player_width,
                player_ext.y,
                player_width,
                player_ext.height,
            };

            // Draw player image and name
            drawPlayer(player, current_player_ext);
            ++index;
        }
    }

    void drawPlayer(const Player& player, const Extends& extends) {
        auto size = OpenGL::getImageDimensions(CLIENT_RES_DIR + "skins/durak_bot.png");
        Extends skin_ext = computeCompactExtends({
            extends.x,
            extends.y + extends.height * 0.3f,
            extends.width,
            extends.height * 0.7f
        }, size.second, size.first);
        OpenGL::drawImage(CLIENT_RES_DIR + "skins/durak_bot.png", skin_ext);

        OpenGL::drawText(player.name, {
            extends.x,
            extends.y,
            extends.width,
            extends.height * 0.3f
        }, glm::vec3(0, 0, 0), TEXTSIZE_MEDIUM);
    }

    Extends getCompactExtends(Extends ext) override {
        return ext;
    }

private:
    Extends computeCompactExtends(const Extends& ext, float image_width, float image_height) {
        float ext_aspect = ext.width / ext.height;
        float image_aspect = image_width / image_height;

        Extends result = ext;

        if (ext_aspect > image_aspect) {
            // Extends is wider than the image aspect ratio
            float new_width = ext.height * image_aspect;
            result.x += (ext.width - new_width) / 2.0f;
            result.width = new_width;
        } else {
            // Extends is taller than the image aspect ratio
            float new_height = ext.width / image_aspect;
            result.y += (ext.height - new_height) / 2.0f;
            result.height = new_height;
        }
        return result;
    }
};


// LobbyNode Implementation
LobbyNode::LobbyNode() {
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
}

Extends LobbyNode::getCompactExtends(Extends ext) {
    return ext;
}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(lobby);
    function(back_button);
    function(ready_button);
    function(settings_button);
}

void LobbyNode::handleAvailableActionUpdate(AvailableActionUpdate update){
    update.ok; //für ready
}


LoginScreenNode::LoginScreenNode(Extends ext){
    placeholder_button = std::make_unique<ButtonNode>("CONNECT");
    placeholder_button->setClickEventCallback([](float x, float y){
        std::cout << "Trying to connet to server..." << std::endl;
        clientID = Network::openConnection("localhost", 42069);

        //place holder: da muss mer den de actual name schicke
        ClientConnectEvent event;
        event.username = "Booger Eater";
        Network::sendMessage(std::make_unique<ClientConnectEvent>(event));
    });
    cast(ButtonNode, placeholder_button)->visible = true;

    OpenGL::setCharacterInputCallback([](char c) {
        std::cout << "De eric muss de login screen no mache und es isch grad " << c << " druckt worde" << std::endl;
    });

    updateExtends(ext);
}

void LoginScreenNode::updateExtends(Extends ext){
    extends = ext;
    placeholder_button->updateExtends({
        ext.x + ext.width * 0.3f,
        ext.y + ext.height * 0.4f,
        ext.width * 0.4f,
        ext.height * 0.2f
    });
}

Extends LoginScreenNode::getCompactExtends(Extends ext){
    return ext;
}

void LoginScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node> &)> function) {
    function(placeholder_button);
}
