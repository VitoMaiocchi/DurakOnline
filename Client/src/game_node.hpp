#pragma once

#include "drawable.hpp"

class GameNode : public TreeNode {
    public:
        GameNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

        /*
        Da chömmer den so züg ane tue wi:
        handleCardUpdate
        handleBattlestate update
        etc

        gaht halt erst wenn message.hpp fertig isch
        */
    private:
        void callForAllChildren(std::function<void(std::shared_ptr<Node>)> function);
};
