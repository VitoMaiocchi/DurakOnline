#include "toplevel_nodes.hpp"
#include "drawable.hpp"

#include "drawable.hpp"
#include "viewport.hpp"

Extends computeCompactExtends(Extends ext, float height, float width);

// LobbyNode
class Lobby : public LeafNode {
private:
    // Base
    std::unique_ptr<RectangleNode> base_rectangle;
    std::unique_ptr<BufferNode> base_rectangle_buffer;
    // Title
    std::unique_ptr<TextNode> lobby_title;

    // Buttons
    std::unique_ptr<LinearStackNode> button_stack;
    std::unique_ptr<BufferNode> button_stack_buffer;

    // Players
    std::list<Player> players;

public:
    Lobby() {
        // Base
        base_rectangle = std::make_unique<RectangleNode>(1.0f, 1.0f, 1.0f);
        // Title
        lobby_title = std::make_unique<TextNode>("LOBBY", 0.0f, 0.0f, 0.0f);

        // Buttons
        button_stack = std::make_unique<LinearStackNode>();
        button_stack->setStackType(STACKDIRECTION_HORIZONTAL, STACKTYPE_SPACED);
        button_stack->children.push_back(createButton("BACK", {1.0f, 0.0f, 0.0f}));
        button_stack->children.push_back(createButton("START GAME", {0.0f, 1.0f, 0.0f}));
        button_stack->children.push_back(createButton("SETTINGS", {0.0f, 0.0f, 1.0f}));
        button_stack_buffer = std::make_unique<BufferNode>();
        button_stack_buffer->setBufferSize(BUFFERTYPE_ABSOLUTE, 100);
        button_stack_buffer->child = std::move(button_stack);
    }

    void draw() override {
        // Rectangle extends
        Extends base_ext = {
            extends.x + extends.width * 0.05f,
            extends.y + extends.height * 0.05f,
            extends.width * 0.9f,
            extends.height *0.9f,
            0
        };

        // Title extends
        Extends title_ext = {
            extends.x + extends.width * 0.2f,
            extends.y + (extends.height / 2),
            extends.width * 0.6f,
            extends.height / 2,
            0
        };

        // Player extends
        Extends player_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.35f,
            extends.width * 0.8f,
            extends.height * 0.2f,
            0
        };

        // Button extends
        Extends button_ext = {
            extends.x,
            extends.y,
            extends.width,
            extends.height / 3,
            0
        };

        // Update extends
        base_rectangle->updateExtends(base_ext);
        lobby_title->updateExtends(title_ext);
        button_stack_buffer->updateExtends(button_ext);

        // Draw
        base_rectangle->draw();
        lobby_title->draw();
        drawPlayers(player_ext);

        button_stack_buffer->draw();
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
        // Draw player skin
        auto size = OpenGL::getImageDimensions(CLIENT_RES_DIR + "skins/durak.png");
        Extends skin_ext = computeCompactExtends({
            extends.x,
            extends.y + extends.height * 0.3f, // Reserve space for text
            extends.width,
            extends.height * 0.7f
        }, size.second, size.first);
        OpenGL::drawImage(CLIENT_RES_DIR + "skins/durak.png", skin_ext);

        // Draw player name
        OpenGL::drawText(player.name, {
            extends.x,
            extends.y,
            extends.width,
            extends.height * 0.3f // Use reserved space
        }, glm::vec3(0, 0, 0), TEXTSIZE_LARGE);
    }

    void updatePlayer(const std::list<Player>& new_players) {
        players = new_players;
    }

    Extends getCompactExtends(Extends ext) override {
        return ext;
    }

private:
    // Helper to create buttons
    std::unique_ptr<TextNode> createButton(const std::string& label, glm::vec3 color) {
        return std::make_unique<TextNode>(label, color.r, color.g, color.b);
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
