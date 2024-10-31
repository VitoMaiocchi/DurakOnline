#pragma once 

#include "util.hpp"
#include "opengl.hpp"
#include <string>
#include <memory>
#include <list>
#include <functional>

class Node {
    public:
        virtual void draw() = 0;
        virtual void updateExtends(Extends ext) = 0;
        virtual Extends getCompactExtends(Extends ext) = 0;

        void sendClickEvent(float x, float y);
        void setClickEventCallback(std::function<void()> callback);
    protected:
        virtual void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) = 0;
        Extends extends = {0,0,0,0,0};
        std::function<void()> clickEventCallback = [](){/*nothing*/};
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

class ImageNode : public LeafNode {
    public:
        ImageNode(std::string path);
        Extends getCompactExtends(Extends ext);
        void draw();

    private:
        OpenGL::Image image;
        uint width, height;
};

class RectangleNode : public LeafNode {
    public:
        RectangleNode(float r, float g, float b);
        Extends getCompactExtends(Extends ext);
        void draw();

    private:
        OpenGL::Rectangle rect;
        float r, g, b;
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

        std::list<std::unique_ptr<Node>> children;
    
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        StackDirection stackDirection = STACKDIRECTION_HORIZONTAL;
        StackType stackType = STACKTYPE_COMPACT;
};