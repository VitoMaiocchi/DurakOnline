#pragma once

#include "drawable.hpp"
#include <Networking/message.hpp>

class GameNode : public TreeNode {
    public:
        GameNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

        //message handling
        void handleCardUpdate(CardUpdate update);
        void handleBattleStateUpdate(BattleStateUpdate update);
        void handleAvailableActionUpdate(AvailableActionUpdate update);
        void playerUpdateNotify();
    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
};
