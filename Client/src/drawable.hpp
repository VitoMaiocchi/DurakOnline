#pragma once 

#include "util.hpp"
#include "opengl.hpp"
#include <string>
#include <memory>
#include <list>
#include <functional>
#include <vector>

#define DEFAULT_TRANSPARANCY 0.1f
#define TEXTFIELD_BORDER 5.0f

/*
The Node Framework Provides a way to stucture anything displayed on screen
in a hirachy of nodes. Any updates in the Window size propagte trough the node tree.
The size of each Node in the tree is only calulated when the window size changes.
This Extends of the Node is used to handle drawing and mouse events like hover/click.
This makes everything scale dynamically

drawable contains any components that can be reused across the atual game nodes.

the abstract Base class Node provides the following functionality:
click events that propagate trough the node tree and call a click callback
a hover bool that is dynamically updated when ever the mouse leaves or enters
the Node

TreeNode is a template for any Nodes that do not draw anything themselves but
only contain other nodes. it implements a draw function that draws all children.

LeafNode is a template for any Node that does not have any children. It implements 
extend updates and the callForAllChildren helper (here it does nothing)

ButtonNode: obvious functionality

PlayerNode: draws a player icon with the correct name
The Lobby version displays if the player is ready
and the game version some information about his game state.

TextInputNode: displays text input including a default text.
*/

class Node {
    public:
        virtual ~Node() = default;
        virtual void draw() = 0;
        virtual void updateExtends(Extends ext) = 0;
        virtual Extends getCompactExtends(Extends ext) = 0;

        virtual void sendClickEvent(float x, float y);
        virtual void sendHoverEvent(float x, float y);
        void setClickEventCallback(std::function<void(float, float)> callback);
    protected:
        virtual void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) = 0;
        Extends extends = {0,0,0,0};
        std::function<void(float, float)> clickEventCallback = [](float x, float y){/*nothing*/};
        bool hover = false;
};

class TreeNode : public Node {
    public:
    void draw();
};

class LeafNode : public Node {
    public:
    void updateExtends(Extends ext);
    protected:
    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
};

class ButtonNode : public LeafNode {
    std::string text;

    public:
    bool visible = true;
    ButtonNode(std::string text);
    Extends getCompactExtends(Extends ext) override;
    void draw() override;
    void sendClickEvent(float x, float y) override;
};

class PlayerNode : public LeafNode {
    const Player* player;
    const bool game; //true for game; false for lobby version

    public:
    PlayerNode(const Player* player, const bool game);
    Extends getCompactExtends(Extends ext);
    void draw();
};

class TextInputNode : public LeafNode {
    std::string placeholder;
    std::string text;
    bool focused = false;

public:
    bool visible = true;

    TextInputNode(const std::string& placeholder) : placeholder(placeholder), text(placeholder) {}

    Extends getCompactExtends(Extends ext) override;
    void draw() override;
    void sendClickEvent(float x, float y) override;
    void handleCharacterInput(char c);

    std::string getText() const {return text;}
    bool isFocused() const {return focused;}
};


