#include "drawable.hpp"
#include "viewport.hpp"
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
    if(!extends.contains(x,y)) return;
    hoverEventCallback(x,y);
    callForAllChildren([x,y](std::unique_ptr<Node>& child){
        child->sendHoverEvent(x,y);
    });
}
void Node::setHoverEventCallback(std::function<void(float, float)> callback) {
    hoverEventCallback = callback;
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
}
