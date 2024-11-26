#pragma once

#include <Networking/message.hpp>
#include "drawable.hpp"
#include <set>

extern ClientID clientID; //client id of this client;

//ONLY ONE MASTER NODE CAN EXIST
//IT IS CREATED BY opengl.cpp 
class MasterNode : public TreeNode {
    public:
        MasterNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
};

void handleMessage(std::unique_ptr<Message> message);

namespace GlobalState {
    //all state information from the server is stored here
    extern GameState game_state;
    extern std::set<Player> players;
}