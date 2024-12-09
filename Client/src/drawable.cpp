#include "drawable.hpp"
#include <cmath>
#include <iostream>

//NODE
void Node::sendClickEvent(float x, float y) {
    if(!extends.contains(x,y)) return;
    clickEventCallback(x,y);
    callForAllChildren([x,y](std::unique_ptr<Node>& child){
        child->sendClickEvent(x,y);
    });
}
void Node::setClickEventCallback(std::function<void(float, float)> callback) {
    clickEventCallback = callback;
}

void Node::sendHoverEvent(float x, float y) {
    hover = extends.contains(x,y);
    callForAllChildren([x,y](std::unique_ptr<Node>& child){
        child->sendHoverEvent(x,y);
    });
}


//LEAF /TREE NODES
void TreeNode::draw() {
    callForAllChildren([](std::unique_ptr<Node>& child) {
        if (child) {  // Ensure child is valid
            child->draw();
        } else {
            std::cerr << "Warning: Attempted to draw a null child node." << std::endl;
        }
    });
}

void LeafNode::updateExtends(Extends ext) {
    extends = getCompactExtends(ext);
}

void LeafNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    //do nothing
}

ButtonNode::ButtonNode(std::string text) : text(text) {} 
    bool visible = false;

Extends ButtonNode::getCompactExtends(Extends ext) {
    return ext;
}

void ButtonNode::draw() { 
    if(!visible) return;
    if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,2*DEFAULT_TRANSPARANCY));
    else OpenGL::drawRectangle(extends, glm::vec4(0,0,0,DEFAULT_TRANSPARANCY));
    OpenGL::drawText(text, extends, glm::vec3(0,0,0), TEXTSIZE_LARGE);
}

PlayerNode::PlayerNode(const Player* player, const bool game) : player(player), game(game) {}

Extends PlayerNode::getCompactExtends(Extends ext) {
    if(ext.height >  ext.width) {
        ext.y += (ext.height-ext.width)/2;
        ext.height = ext.width;
    } else {
        ext.x += (ext.width-ext.height)/2;
        ext.width = ext.height;
    }
    return ext;
}

inline std::string getPlayerIconPath(const std::string &name) {
    std::string path = "durak.png";
    if(name == "danil" || name == "Danil") path = "player.png";
    if(name == "thomas" || name == "Thomas") path = "clown.png";
    if(name == "vito" || name == "Vito") path = "chad.jpg";
    if(name == "eric" || name == "Eric") path = "chad.jpg";
    return CLIENT_RES_DIR + "skins/" + path;
}

void drawGamePlayer(Extends extends, const std::string &name, const uint cards, const PlayerState state) {
    const std::string path = getPlayerIconPath(name);
    auto size = OpenGL::getImageDimensions(path);
    Extends ext = computeCompactExtends({
        extends.x,
        extends.y + extends.height * 0.3f,
        extends.width,
        extends.height * 0.7f
    }, size.second, size.first);
    OpenGL::drawImage(path, ext);

    size = OpenGL::getImageDimensions(CLIENT_RES_DIR + "icons/hand.png");
    OpenGL::drawImage(CLIENT_RES_DIR + "icons/hand.png", computeCompactExtends({
        extends.x + extends.width * 0.15f,
        extends.y + extends.height * 0.15f,
        extends.width * 0.2f,
        extends.height * 0.15f
    }, size.second, size.first));

    OpenGL::drawText(std::to_string(cards), {
        extends.x + extends.width * 0.35f,
        extends.y + extends.height * 0.15f,
        extends.width * 0.2f,
        extends.height * 0.15f
    }, glm::vec3(0,0,0), TEXTSIZE_LARGE, TEXTALIGN_LEFT);

    if(state != PLAYERSTATE_NONE) {
        const std::string s = getPlayerStateIcon(state);
        size = OpenGL::getImageDimensions(s);
        OpenGL::drawImage(s, computeCompactExtends({
            extends.x + extends.width * 0.65f,
            extends.y + extends.height * 0.15f,
            extends.width * 0.2f,
            extends.height * 0.15f
        }, size.second, size.first));
    }

    OpenGL::drawText(name, {
        extends.x,
        extends.y,
        extends.width,
        extends.height * 0.15f
    }, glm::vec3(0,0,0), TEXTSIZE_MEDIUM);
}

void drawLobbyPlayer(Extends extends, const std::string &name) {
    const std::string path = getPlayerIconPath(name);
    auto size = OpenGL::getImageDimensions(path);

    Extends ext = computeCompactExtends({
        extends.x,
        extends.y + extends.height * 0.15f,
        extends.width,
        extends.height * 0.85f
    }, size.second, size.first);
    OpenGL::drawImage(path, ext);

    OpenGL::drawText(name, {
        extends.x,
        extends.y,
        extends.width,
        extends.height * 0.15f
    }, glm::vec3(0,0,0), TEXTSIZE_MEDIUM);
}

void PlayerNode::draw() {
    if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,DEFAULT_TRANSPARANCY));
    if(game) drawGamePlayer(extends, player->name, player->game->cards, player->game->state);
    else drawLobbyPlayer(extends, player->name);
}

Extends TextInputNode::getCompactExtends(Extends ext) {
    return ext;
}

void TextInputNode::draw() {
    if (!visible) return;

    //outline of the button
    OpenGL::drawRectangle(extends, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
    //button
    glm::vec4 backgroundColor = focused ? glm::vec4(0.8, 0.8, 0.8, 1.0) : glm::vec4(1.0, 1.0, 1.0, 1.0);
    Extends reduced_extends = {
        extends.x + extends.width * 0.0075f,
        extends.y + extends.height * 0.05f,
        extends.width * 0.985f,
        extends.height * 0.9f,
    };
    OpenGL::drawRectangle(reduced_extends, backgroundColor);

    // Draw the text within the field
    glm::vec3 textColor = glm::vec3(0, 0, 0);
    if(text == placeholder){
        textColor = glm::vec3(0.5, 0.5, 0.5);
    }
    OpenGL::drawText(text, extends, textColor, TEXTSIZE_LARGE);
}

void TextInputNode::sendClickEvent(float x, float y) {
    if (x >= extends.x && x <= extends.x + extends.width &&
        y >= extends.y && y <= extends.y + extends.height) {
        focused = true;
    } else {
        focused = false;
    }
}

void TextInputNode::handleCharacterInput(char c) {
    if (focused) {
        if (c == '\b') {
            //delete
            if (!text.empty()) {
                text.pop_back();
            }
        } else if (std::isprint(c)) {
            if(text == placeholder) text.clear();
            text.push_back(c);
        }
    }
}

