#include "drawable.hpp"
#include <cmath>

//NODE
void Node::draw(Extends ext) {
    if(ext == last_ext) {
        drawPrevious();
    } else {
        last_ext = ext;
        drawNew(ext);
    }
}

void Node::sendClickEvent(float x, float y) {
    if(!last_ext.contains(x,y)) return;
    clickEventCallback();
    callForAllChildren([x,y](std::shared_ptr<Node> child){
        child->sendClickEvent(x,y);
    });
}
void Node::setClickEventCallback(std::function<void()> callback) {
    clickEventCallback = callback;
}

//LEAF / TREE NODES
void TreeNode::drawPrevious() {
    callForAllChildren([](std::shared_ptr<Node> child){
        child->drawPrevious();
    });
}

void LeafNode::drawPrevious() {
    drawNew(last_ext);
}

void LeafNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    //do nothing
}

//IMAGE NODE

ImageNode::ImageNode(std::string path) : image(path) {
    image.getDimensions(width, height);
}

void ImageNode::drawNew(Extends ext) {
    bool heightb = (float)ext.height / ext.width < (float)height / width;
    float size = heightb ? ext.height : ext.width;
    image.draw(ext.x + ext.width/2.0f, ext.y + ext.height/2.0f, size, heightb, ext.layer);
}

uint ImageNode::minWidth(uint height) {
    return std::ceil( height/this->height * width);
}

uint ImageNode::minHeight(uint width) {
    return std::ceil( width/this->width * height );
}

//BUFER NODE

BufferNode::BufferNode() {
    bufferType = BUFFERTYPE_ABSOLUTE;
    bufferSize = 0;
    child = NULL;
}

void BufferNode::drawNew(Extends ext) {
    if(!child) return;
    switch (bufferType) {
        case BUFFERTYPE_ABSOLUTE:
        ext.x += bufferSize;
        ext.y += bufferSize;
        if(ext.width > bufferSize*2) ext.width -= bufferSize*2;
        else ext.width = 0;
        if(ext.height > bufferSize*2) ext.height -= bufferSize*2;
        else ext.height = 0;
        break;
    }
    child->draw(ext);
}

uint BufferNode::minWidth(uint height) {
    if(!child) return 0;
    switch (bufferType) {
        case BUFFERTYPE_ABSOLUTE:
        //TODO
        break;
    }
}

uint BufferNode::minHeight(uint width) {
    if(!child) return 0;
    switch (bufferType) {
        case BUFFERTYPE_ABSOLUTE:
        //TODO
        break;
    }
}

void BufferNode::setBufferSize(BufferType buffer_type, float buffer_size) {
    bufferType = buffer_type;
    bufferSize = buffer_size;
}

void BufferNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    function(child);
}

//LINEAR STACK NODE
void LinearStackNode::drawNew(Extends ext) {

}

uint LinearStackNode::minWidth(uint height) {

}

uint LinearStackNode::minHeight(uint width) {

}

void LinearStackNode::setStackType(StackDirection stack_direction, StackType stack_type) {
    stackDirection = stack_direction;
    stackType = stack_type;
}

void LinearStackNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    for(auto child : children) function(child);
}