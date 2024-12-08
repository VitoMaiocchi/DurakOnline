#pragma once 

#include "util.hpp"
#include "opengl.hpp"
#include <string>
#include <memory>
#include <list>
#include <functional>
#include <vector>

#define DEFAULT_TRANSPARANCY 0.1f

class Node {
    public:
        bool visible = false;
        virtual void draw() = 0;
        virtual void updateExtends(Extends ext) = 0;
        virtual Extends getCompactExtends(Extends ext) = 0;

        virtual void sendClickEvent(float x, float y);
        virtual void sendHoverEvent(float x, float y);
        virtual void handleCharacterInput(char c) {}
        virtual std::string getText() const { return ""; } // Default implementation
        virtual bool isFocused() const {return false;}
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
    Extends getCompactExtends(Extends ext);
    void draw();
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
    void handleCharacterInput(char c) override;

    std::string getText() const override{return text;}
    bool isFocused() const {return focused;}
};


