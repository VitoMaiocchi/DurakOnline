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

Extends computeCompactExtends(Extends ext, float height, float width) {
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

//IMAGE NODE

ImageNode::ImageNode(std::string path) : image(path) {
    image.getDimensions(width, height);
}

void ImageNode::draw() {
    image.draw(extends);
}

Extends ImageNode::getCompactExtends(Extends ext) {
    return computeCompactExtends(ext, height, width);
}

RectangleNode::RectangleNode(float r, float g, float b) : rect(r, g, b) {}
Extends RectangleNode::getCompactExtends(Extends ext) {
    return ext;
}
void RectangleNode::draw() {
    rect.draw(extends);
}

//TEXT NODE
TextNode::TextNode(std::string text, float r, float g, float b) : text(text, r, g, b) {}
void TextNode::updateContent(std::string text) {
    this->text.text = text;
}

Extends TextNode::getCompactExtends(Extends ext) {
    float width, height;
    text.getSize(width, height);
    return computeCompactExtends(ext, height, width);
}

void TextNode::draw() { //TEXT SIZE MAX
    float width, height;
    text.getSize(width, height);
    Extends ext = computeCompactExtends(extends, height, width);
    float scale_factor = ext.width / width;
    text.draw(extends.x, extends.y, scale_factor);
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
//TODO STACKTYPE
void LinearStackNode::updateExtends(Extends ext) {
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

            for (auto& child : children) {
                Extends childExt = ext;

                childExt.x += offset;
                childExt.width = child->getCompactExtends(ext).width;
                childExt.y = (ext.height-childExt.height*scale)/2.;
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

            for (auto& child : children) {
                Extends childExt = ext;

                childExt.y += offset;
                childExt.height = child->getCompactExtends(ext).height;
                childExt.x = (ext.width-childExt.width*scale)/2.;
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

//TODO STACKTYPE
Extends LinearStackNode::getCompactExtends(Extends ext) {
    float totalWidth = 0.0f;
    float totalHeight = 0.0f;

    if (stackDirection == STACKDIRECTION_HORIZONTAL) {
        for (const auto& child : children) {
            Extends childExt = child->getCompactExtends(ext);
            totalWidth += childExt.width;
            totalHeight = std::max(totalHeight, childExt.height);
        }
    } else {
        for (const auto& child : children) {
            Extends childExt = child->getCompactExtends(ext);
            totalHeight += childExt.height;
            totalWidth = std::max(totalWidth, childExt.width);
        }
    }

    float offsetX = ext.x + (ext.width - totalWidth)/2;
    float offsetY = ext.y + (ext.height - totalHeight)/2;

    return {offsetX, offsetY, totalWidth, totalHeight};
}

void LinearStackNode::setStackType(StackDirection stack_direction, StackType stack_type) {
    stackDirection = stack_direction;
    stackType = stack_type;
}

void LinearStackNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    for(auto& child : children) function(child);
}
