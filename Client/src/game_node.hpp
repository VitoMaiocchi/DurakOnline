#pragma once

#include "drawable.hpp"
#include <Networking/message.hpp>
#include <memory>

/*
The GameNode is responisble for displaying everything that happens during the actual game.
allmost all state updates from the server are passed on to here.
*/

class GameNode : public TreeNode {
    public:
        GameNode(Extends ext);
        virtual ~GameNode() = default;
        void updateExtends(Extends ext) override;
        Extends getCompactExtends(Extends ext) override;

        //Any time any card get changed this update gets called.
        //the function updates all cards in the middle, in your hand,
        //the trump card, the deck amount
        //and the amount of cards the enemies have
        void handleCardUpdate(CardUpdate update);
        //this updates who of the players is attacker, defender, etc
        void handleBattleStateUpdate(BattleStateUpdate update);
        //this updates what buttons are displayed (done, pickup, reflect)
        void handleAvailableActionUpdate(AvailableActionUpdate update);
        //notifies game that the player set has changed
        void playerUpdateNotify();
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) override;
        std::unique_ptr<Node> handNode;         //displays the player hand at the bottom
        std::unique_ptr<Node> middleNode;       //displays the cards in the middle
        std::unique_ptr<Node> playerBarNode;    //displays the enemies at the top
        std::unique_ptr<Node> playerActionNode; //displays the action buttons on the bottom right
        std::unique_ptr<Node> deckNode;         //displays information on the deck and trump card
        std::unique_ptr<Node> playerStateNode;  //displays the text informing the player on their defend/attack state
};
