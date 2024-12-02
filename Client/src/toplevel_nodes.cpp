#include "toplevel_nodes.hpp"
#include "drawable.hpp"
#include "master_node.hpp"

// LobbyNode
class Lobby : public LeafNode {
private:
    // Buttons
    struct Button {
        std::string label;
        glm::vec3 color;
        Extends extends;
    };
    std::vector<Button> buttons;

    // Players
    std::list<Player> players;

public:
    Lobby() {
        // Buttons
        buttons.push_back({"BACK", {0.0f, 0.0f, 0.0f}, {}});
        buttons.push_back({"READY", {0.0f, 0.0f, 0.0f}, {}});
        buttons.push_back({"SETTINGS", {0.0f, 0.0f, 0.0f}, {}});
    }

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

        // Button extends
        Extends button_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.1f,
            extends.width * 0.8f,
            extends.height * 0.2f,
        };
        drawButtons(button_ext);
    }

    void drawPlayers(const Extends& player_ext) {
        // Number of players
        int num_players = players.size();
        if (num_players == 0) return;

        // Calculate the width for each player (horizontal layout)
        float player_width = player_ext.width / num_players;

        // Loop over each player and draw
        int index = 0;
        for (const auto& player : players) {
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
        }, glm::vec3(0, 0, 0), TEXTSIZE_LARGE);
    }

    void drawButtons(const Extends& button_area_ext) {
        int num_buttons = buttons.size();
        if (num_buttons == 0) return;

        // Calculate the width
        float button_width = button_area_ext.width / num_buttons;

        int index = 0;
        for (auto& button : buttons) {
            float middle = 0.;
            if(index == 1) middle = (0.3f*button_width)*(1/6.);
            if(index == 2) middle = (0.3f*button_width)*(2/6.);
            // Calculate the extends for the current button
            Extends button_ext = {
                button_area_ext.x + index * button_width + middle,
                button_area_ext.y,
                button_width*0.9f,
                button_area_ext.height*0.4f,
            };
            button.extends = button_ext;

            // Draw button background
            OpenGL::drawRectangle(button_ext, glm::vec4(0.9f, 0.4f, 0.4f, 0.4f));

            // Draw button label
            OpenGL::drawText(button.label, {
                button_ext.x,
                button_ext.y + button_ext.height * 0.3f,
                button_ext.width,
                button_ext.height * 0.4f
            }, glm::vec3(1.0f, 1.0f, 1.0f), TEXTSIZE_LARGE);

            ++index;
        }
    }

    void updatePlayer(const std::list<Player>& new_players) {
        players = new_players;
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
}

void LobbyNode::updateExtends(Extends ext) {
    lobby->updateExtends(ext);
}

Extends LobbyNode::getCompactExtends(Extends ext) {
    return ext;
}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(lobby);
}

void LobbyNode::playerUpdateNotify() {
    GlobalState::players; //-> set of current players
}

void LobbyNode::handleAvailableActionUpdate(AvailableActionUpdate update){
    update.ok; //für ready
}