#pragma once

#include "drawable.hpp"

//ONLY ONE MASTER NODE CAN EXIST
//IT IS CREATED BY opengl.cpp 
class MasterNode : public TreeNode {
    public:
        MasterNode();
        uint minWidth(uint height);
        uint minHeight(uint width);
    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
        void drawNew(Extends ext);
};