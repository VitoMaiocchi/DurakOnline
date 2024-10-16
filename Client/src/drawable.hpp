#pragma once 

#include "util.hpp"
#include "image.hpp"
#include <string>
#include <memory>
#include <list>
#include <functional>

class Node {
    public:
        void draw(Extends ext);
        virtual uint minWidth(uint height) = 0;
        virtual uint minHeight(uint width) = 0;
        virtual void drawPrevious() = 0;

        void sendClickEvent(float x, float y);
        void setClickEventCallback(std::function<void()> callback);
    protected:
        virtual void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) = 0;
        virtual void drawNew(Extends ext) = 0;
        Extends last_ext = {0,0,0,0,0};
        std::function<void()> clickEventCallback = [](){/*nothing*/};
};

class TreeNode : public Node {
    void drawPrevious();
};

class LeafNode : public Node {
    void drawPrevious();
    void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
};

class ImageNode : public LeafNode {
    public:
        ImageNode(std::string path);
        uint minWidth(uint height);
        uint minHeight(uint width);

    private:
        void drawNew(Extends ext);
        Image image;
        uint width, height;
};

//BIS JETZT NUR BUFFERTYPE ABSOLUTE IMPLEMENTIERT: 
//will ich bin lazy und ich machs nur wenns öpper brucht
enum BufferType {
    BUFFERTYPE_ABSOLUTE,        //absolute pixel amount
    BUFFERTYPE_HIGHT_RELATIVE,  //float zwüsche 0.0 und 1.0
    BUFFERTYPE_WIDTH_RELATIVE   //float zwüsche 0.0 und 1.0
};

class BufferNode : public TreeNode {
    public:
        BufferNode();
        uint minWidth(uint height);
        uint minHeight(uint width);
        void setBufferSize(BufferType buffer_type, float buffer_size);

        std::shared_ptr<Node> child;

    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
        void drawNew(Extends ext);
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
        uint minWidth(uint height);
        uint minHeight(uint width);
        void setStackType(StackDirection stack_direction, StackType stack_type);

        std::list<std::shared_ptr<Node>> children;
    
    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
        void drawNew(Extends ext);
        StackDirection stackDirection = STACKDIRECTION_HORIZONTAL;
        StackType stackType = STACKTYPE_COMPACT;
};