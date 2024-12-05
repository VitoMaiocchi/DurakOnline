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
    if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,0.25));
    else OpenGL::drawRectangle(extends, glm::vec4(0,0,0,0.15));
    OpenGL::drawText(text, extends, glm::vec3(0,0,0), TEXTSIZE_LARGE);
}