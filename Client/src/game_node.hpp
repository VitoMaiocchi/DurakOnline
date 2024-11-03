#pragma once

#include "drawable.hpp"
#include <Networking/message.hpp>
#include <memory>

class GameNode : public TreeNode {
    public:
        GameNode();
        virtual ~GameNode() = default;
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

        //message handling
        void handleCardUpdate(CardUpdate update);
        void handleBattleStateUpdate(BattleStateUpdate update);
        void handleAvailableActionUpdate(AvailableActionUpdate update);
        void playerUpdateNotify();
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> handNode;
};
