#include "drawable.hpp"
#include "viewport.hpp"
#include <cmath>
#include <iostream>

//NODE
void Node::sendClickEvent(float x, float y) {
    if(!extends.contains(x,y)) return;
    clickEventCallback();
    callForAllChildren([x,y](std::unique_ptr<Node>& child){
        child->sendClickEvent(x,y);
    });
}
void Node::setClickEventCallback(std::function<void()> callback) {
    clickEventCallback = callback;
}

//LEAF /TREE NODES
void TreeNode::draw() {
    callForAllChildren([](std::unique_ptr<Node>& child){
        child->draw();
    });
}

void LeafNode::updateExtends(Extends ext) {
    extends = getCompactExtends(ext);
}

void LeafNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    //do nothing
}

//IMAGE NODE

ImageNode::ImageNode(std::string path) : image(path) {
    image.getDimensions(width, height);
}

void ImageNode::draw() {
    image.draw(extends);
}

Extends ImageNode::getCompactExtends(Extends ext) {
    float h, w;
    if( (float)ext.height / ext.width < (float)height / width ) { //height
        h = ext.height;
        w = (float) width * ext.height / height;
    } else { //width
        w = ext.width;
        h = (float) height * ext.width / width;
    }

    return {
        ext.x + (ext.width - w)/2,
        ext.y + (ext.height - h)/2,
        w,
        h
    };
}

RectangleNode::RectangleNode(float r, float g, float b) : rect(r, g, b) {}
Extends RectangleNode::getCompactExtends(Extends ext) {
    return ext;
}
void RectangleNode::draw() {
    rect.draw(extends);
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
    extends = {ext.x - s, ext.y - s, ext.width+2*s, ext.height+2*s, ext.layer};
}

Extends BufferNode::getCompactExtends(Extends ext) {
    if(!child) return {0,0,0,0,ext.layer};
    ext = getCompactChildExt(ext);
    float s = getBufferSize();
    return {ext.x - s, ext.y - s, ext.width+2*s, ext.height+2*s, ext.layer};
}

void BufferNode::setBufferSize(BufferType buffer_type, float buffer_size) {
    bufferType = buffer_type;
    bufferSize = buffer_size;
}

void BufferNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(child);
}

//LINEAR STACK NODE
void LinearStackNode::updateExtends(Extends ext) {
    //TODO
}

Extends LinearStackNode::getCompactExtends(Extends ext) {
    return ext; //TODO
}

void LinearStackNode::setStackType(StackDirection stack_direction, StackType stack_type) {
    stackDirection = stack_direction;
    stackType = stack_type;
}

void LinearStackNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    for(auto& child : children) function(child);
}
