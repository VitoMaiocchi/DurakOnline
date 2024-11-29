#include "toplevel_nodes.hpp"
<<<<<<< HEAD
#include "master_node.hpp"
#include "viewport.hpp"

/*
das isch alles vorher in drawable gsi. Es sind halt helper.
Ich hans süsch nirgens benuzt drum han ichs jetz da ine ta. (damits nöd s drawable file zue cluttered)
Du chasch efi alli wo du da no bruchsch benuze. (de rescht chammer lösche damits nöd so voll mit müll isch)
Stack und Buffer sind sicher nützlich die leaf nodes würi nöd bruche
di sind schwach sinn hani gmerkt.
*/

//IMAGE RECTANGLE UND TEXT NODE SIND OBSOLET LIBER DIREKT DRAW MACHE
class ImageNode : public LeafNode {
    public:
        ImageNode(std::string path);
        Extends getCompactExtends(Extends ext);
        void draw();

    private:
        uint width, height;
        std::string path;
};

class RectangleNode : public LeafNode {
    public:
        RectangleNode(float r, float g, float b);
        Extends getCompactExtends(Extends ext);
        void draw();

    private:
        float r, g, b;
};

class TextNode : public LeafNode {
    public:
        TextNode(std::string text, float r, float g, float b);
        void updateContent(std::string text);
        Extends getCompactExtends(Extends ext);
        void draw();
        std::string text;
        glm::vec3 color;
};


enum BufferType {
    BUFFERTYPE_ABSOLUTE,
    BUFFERTYPE_RELATIVE
};

class BufferNode : public TreeNode {
    public:
        BufferNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
        void setBufferSize(BufferType buffer_type, float buffer_size);

        std::unique_ptr<Node> child;

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        float getBufferSize();
        Extends getCompactChildExt(Extends ext);
        BufferType bufferType;
        float bufferSize;
};

enum StackDirection {
    STACKDIRECTION_HORIZONTAL,
    STACKDIRECTION_VERTICAL
};

enum StackType {
    STACKTYPE_COMPACT,
    STACKTYPE_SPACED
};

class LinearStackNode : public TreeNode {
    public:
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
        void setStackType(StackDirection stack_direction, StackType stack_type);

        std::vector<std::unique_ptr<Node>> children;
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        StackDirection stackDirection = STACKDIRECTION_HORIZONTAL;
        StackType stackType = STACKTYPE_COMPACT;
};


LobbyNode::LobbyNode(){
}

void LobbyNode::updateExtends(Extends ext){
}

Extends LobbyNode::getCompactExtends(Extends ext){
    return ext;
}

void LobbyNode::playerUpdateNotify() {
    GlobalState::players; //-> set of current players
}

void LobbyNode::handleAvailableActionUpdate(AvailableActionUpdate update){
}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node> &)> function){
}



LoginScreenNode::LoginScreenNode(){
}

void LoginScreenNode::updateExtends(Extends ext){
}

Extends LoginScreenNode::getCompactExtends(Extends ext){
    return ext;
}

void LoginScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node> &)> function) {
}




GameOverScreen::GameOverScreen(bool durak) : durak(durak) {
}

void GameOverScreen::updateExtends(Extends ext) {
}

Extends GameOverScreen::getCompactExtends(Extends ext) {
    return ext;
}

void GameOverScreen::callForAllChildren(std::function<void(std::unique_ptr<Node> &)> function) {
}


//IMAGE NODE

ImageNode::ImageNode(std::string path) : path(path) {
    auto dim = OpenGL::getImageDimensions(path);
    width = dim.first;
    height = dim.second;
}

void ImageNode::draw() {
    OpenGL::drawImage(path, extends);
}

Extends ImageNode::getCompactExtends(Extends ext) {
    return computeCompactExtends(ext, height, width);
}

RectangleNode::RectangleNode(float r, float g, float b) : r(r), g(g), b(b) {}
Extends RectangleNode::getCompactExtends(Extends ext) {
    return ext;
}
void RectangleNode::draw() {
    OpenGL::drawRectangle(extends, glm::vec4(r,g,b,1.0f));
}

//TEXT NODE
TextNode::TextNode(std::string text, float r, float g, float b) : text(text), color(r,g,b) {}
void TextNode::updateContent(std::string text) {
    this->text = text;
}

Extends TextNode::getCompactExtends(Extends ext) {
    return ext; //nöd richtig so aber will das eh weg rationalisiere
}

void TextNode::draw() { //TEXT SIZE MAX
    OpenGL::drawText(text, extends, color, TEXTSIZE_LARGE);
}


//BUFER NODE
BufferNode::BufferNode() {
    bufferType = BUFFERTYPE_ABSOLUTE;
    bufferSize = 0;
    child = NULL;
}

float BufferNode::getBufferSize() {
    if(bufferType == BUFFERTYPE_ABSOLUTE) return bufferSize;
    return bufferSize * Viewport::global_scalefactor;
}

//TODO: das gfallt mir nonig so weg code duplication und so
Extends BufferNode::getCompactChildExt(Extends ext) {
    if(!child) throw std::runtime_error("getCompactChildExt: child is NULL");

    float s = getBufferSize();
    ext.x += s;
    ext.y += s;
    if(ext.width > s*2) ext.width -= s*2;
    else ext.width = 0;
    if(ext.height > s*2) ext.height -= s*2;
    else ext.height = 0;

    return child->getCompactExtends(ext);
}

void BufferNode::updateExtends(Extends ext) {
    if(!child) {
        extends = ext;
        return;
    }
    ext = getCompactChildExt(ext);
    float s = getBufferSize();
    child->updateExtends(ext);
    extends = {ext.x - s, ext.y - s, ext.width+2*s, ext.height+2*s};
}

Extends BufferNode::getCompactExtends(Extends ext) {
    if(!child) return {0,0,0,0};
    ext = getCompactChildExt(ext);
    float s = getBufferSize();
    return {ext.x - s, ext.y - s, ext.width+2*s, ext.height+2*s};
}

void BufferNode::setBufferSize(BufferType buffer_type, float buffer_size) {
    bufferType = buffer_type;
    bufferSize = buffer_size;
}

void BufferNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(child);
}

//LINEAR STACK NODE

#define MAX_FLOAT 10E10 //todo

void LinearStackNode::updateExtends(Extends ext) {

    if(stackType == STACKTYPE_COMPACT) {
        ext = getCompactExtends(ext);

        float y = ext.y;
        float x = ext.x;
        Extends cext;
        for(auto& child : children) {
            if(stackDirection == STACKDIRECTION_VERTICAL) cext = child->getCompactExtends({0,0, ext.width, MAX_FLOAT});
            else cext = child->getCompactExtends({0,0, MAX_FLOAT, ext.height});

            cext.y = y;
            cext.x = x;
            child->updateExtends(cext);

            if(stackDirection == STACKDIRECTION_VERTICAL) y += cext.height;
            else x += cext.width;
        }
        return;
    }


    //legacy eric code (funktioniert wahschinlich immer nonig)

    float divisor = 2.0f;
    float offset = 0.0f;
    float offset_space = 0.0f;
    float scale = 0.0f;
    float totalWidth = 0.0f;
    float totalHeight = 0.0f;
    float ratioWidth = 0.0f;
    float ratioHeight = 0.0f;

    switch(stackDirection){
        case(STACKDIRECTION_HORIZONTAL):
            for (auto& child : children) {
                totalWidth += child->getCompactExtends(ext).width;
                totalHeight = std::max(totalHeight, child->getCompactExtends(ext).height);
            }

            ratioWidth = ext.width/totalWidth;
            ratioHeight = ext.height/totalHeight;
            scale = std::min(ratioHeight, ratioWidth);

            offset = ((ext.width - totalWidth*scale)/2);

            if(stackType == STACKTYPE_SPACED){
                offset_space = ((ext.width - totalWidth*scale)/(children.size()-1));
                offset = 0.0;
            }

            offset += ext.x;

            for (auto& child : children) {
                Extends childExt = ext;

                childExt.x += offset;
                childExt.width = child->getCompactExtends(ext).width;
                childExt.y = (ext.height-childExt.height*scale)/2. + ext.y;
                childExt.width *= scale;
                childExt.height *= scale;
                offset += childExt.width;

                if(stackType == STACKTYPE_SPACED){
                    offset += offset_space;
                }

                child->updateExtends(childExt);
            }
        break;
        case(STACKDIRECTION_VERTICAL):
            for (auto& child : children) {
                totalWidth = std::max(totalWidth, child->getCompactExtends(ext).width);
                totalHeight += child->getCompactExtends(ext).height;
            }

            ratioWidth = ext.width/totalWidth;
            ratioHeight = ext.height/totalHeight;
            scale = std::min(ratioHeight, ratioWidth);

            offset = ((ext.height - totalHeight*scale)/2);

            if(stackType == STACKTYPE_SPACED){
                offset_space = ((ext.height - totalHeight*scale)/(children.size()-1));
                offset = 0.0;
            }

            offset += ext.y;

            for (auto& child : children) {
                Extends childExt = ext;

                childExt.y += offset;
                childExt.height = child->getCompactExtends(ext).height;
                childExt.x = (ext.width-childExt.width*scale)/2. + ext.x;
                childExt.width *= scale;
                childExt.height *= scale;
                offset += childExt.height;

                if(stackType == STACKTYPE_SPACED){
                    offset += offset_space;
                }

                child->updateExtends(childExt);
            }
        break;
    }
}

Extends LinearStackNode::getCompactExtends(Extends ext) {
    float totalWidth = 0.0f;
    float totalHeight = 0.0f;
    float compact_x = 0.0f;
    float compact_y = 0.0f;

    switch(stackType){
        case(STACKTYPE_COMPACT):
            if (stackDirection == STACKDIRECTION_HORIZONTAL) {
                for (const auto& child : children) totalWidth += child->getCompactExtends({0,0, MAX_FLOAT, ext.height}).width;
                if(totalWidth > ext.width) {
                    totalHeight = ext.height * ext.width / totalWidth;
                    totalWidth = ext.width;
                } else totalHeight = ext.height;
            } else {
                for (const auto& child : children) totalHeight += child->getCompactExtends({0,0, ext.width, MAX_FLOAT}).height;
                if(totalHeight > ext.height) {
                    totalWidth = ext.width * ext.height / totalHeight;
                    totalHeight = ext.height;
                } else totalWidth = ext.width;
            }
            compact_x = ext.x + (ext.width - totalWidth)/2;
            compact_y = ext.y + (ext.height - totalHeight)/2;
        break;
        case(STACKTYPE_SPACED):
        if (stackDirection == STACKDIRECTION_HORIZONTAL) {
            totalWidth = this->extends.width;
            for (const auto& child : children) {
                Extends childExt = child->getCompactExtends(ext);
                totalHeight = std::max(totalHeight, childExt.height);
            }
            compact_x = this->extends.x;
            compact_y = ext.y + (ext.height - totalHeight)/2;
        } else {
            totalHeight = this->extends.height;
            for (const auto& child : children) {
                Extends childExt = child->getCompactExtends(ext);
                totalWidth = std::max(totalWidth, childExt.width);
            }
            compact_x = ext.x + (ext.width - totalWidth)/2;
            compact_y = this->extends.y;
        }
        break;
    }
    
    return {compact_x, compact_y, totalWidth, totalHeight};
}

void LinearStackNode::setStackType(StackDirection stack_direction, StackType stack_type) {
    stackDirection = stack_direction;
    stackType = stack_type;
}

void LinearStackNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    for(auto& child : children) function(child);
=======
#include "drawable.hpp"

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
            0
        };
        OpenGL::drawRectangle(base_ext, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Title extends
        Extends title_ext = {
            extends.x + extends.width * 0.15f,
            extends.y + extends.height * 0.6f,
            extends.width * 0.7f,
            extends.height * 0.3f,
            0
        };
        OpenGL::drawText("LOBBY", title_ext, glm::vec3(0.0f, 0.0f, 0.0f), TEXTSIZE_XLARGE);

        // Player extends
        Extends player_ext = {
            extends.x + extends.width * 0.1f,
            extends.y + extends.height * 0.3f,
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
                0
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
>>>>>>> newLobby
}
