#include "toplevel_nodes.hpp"
#include "drawable.hpp"

// LobbyNode
class Lobby : public LeafNode {
private:
    // Base
    std::unique_ptr<RectangleNode> base_rectangle;
    // Title
    std::unique_ptr<TextNode> lobby_title;

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
        // Base
        base_rectangle = std::make_unique<RectangleNode>(1.0f, 1.0f, 1.0f);
        // Title
        lobby_title = std::make_unique<TextNode>("LOBBY", 0.0f, 0.0f, 0.0f);

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
            0
        };
        base_rectangle->updateExtends(base_ext);
        base_rectangle->draw();

        // Title extends
        Extends title_ext = {
            extends.x + extends.width * 0.2f,
            extends.y + extends.height * 0.7f,
            extends.width * 0.6f,
            extends.height * 0.25f,
            0
        };
        lobby_title->updateExtends(title_ext);
        lobby_title->draw();

        // Player extends
        Extends player_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.4f,
            extends.width * 0.8f,
            extends.height * 0.25f,
            0
        };
        drawPlayers(player_ext);

        // Button extends
        Extends button_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.1f,
            extends.width * 0.8f,
            extends.height * 0.2f,
            0
        };
        drawButtons(button_ext);
    }

    void drawPlayers(const Extends& player_ext) {
        // Number of players
        size_t num_players = players.size();
        if (num_players == 0) return;

        // Calculate the width for each player (horizontal layout)
        float player_width = player_ext.width / num_players;

        // Loop over each player and draw
        size_t index = 0;
        for (const auto& player : players) {
            // Calculate the extends for the current player
            Extends current_player_ext = {
                player_ext.x + index * player_width,
                player_ext.y,
                player_width,
                player_ext.height,
                0
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
        size_t num_buttons = buttons.size();
        if (num_buttons == 0) return;

        // Calculate the width
        float button_width = button_area_ext.width / num_buttons;

        size_t index = 0;
        for (auto& button : buttons) {
            // Calculate the extends for the current button
            Extends button_ext = {
                button_area_ext.x + index * button_width,
                button_area_ext.y,
                button_width*0.9f,
                button_area_ext.height*0.4f,
                0
            };
            button.extends = button_ext;

            // Draw button background
            OpenGL::drawRectangle(button_ext, glm::vec4(0.4,0.2,0.2,0.4));

            // Draw button label
            OpenGL::drawText(button.label, {
                button_ext.x,
                button_ext.y + button_ext.height * 0.3f,
                button_ext.width,
                button_ext.height * 0.4f
            }, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_LARGE);

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
    // Compute compact extends to maintain aspect ratio
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

void LobbyNode::handlePlayerUpdate(PlayerUpdate update) {
    std::list<Player> players;

    for (const auto& [client_id, name] : update.player_names) {
        Player player{client_id, name, client_id == update.durak, false};
        players.push_back(player);
    }

    auto lobbyPtr = dynamic_cast<Lobby*>(lobby.get());
    lobbyPtr->updatePlayer(players);
}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(lobby);
}
