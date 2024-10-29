#pragma once

#include <Networking/message.hpp>
#include "drawable.hpp"

extern uint clientID;

//ONLY ONE MASTER NODE CAN EXIST
//IT IS CREATED BY opengl.cpp 
class MasterNode : public TreeNode {
    public:
        MasterNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
};

void handleMessage(std::unique_ptr<Message> message);