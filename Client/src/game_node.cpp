#include "game_node.hpp"
#include "master_node.hpp"
#include <iostream>

//TODO 
//da chunt alles ine wo grendered wird während es game lauft
//endscreen und so nöd nur das mit de charte i de mitti und so

void sortCards(std::list<Card> &cards) {
    //TODO
    //da muss mer wüsse was trumpf isch und so
}

void drawCard(float x, float y, float height, Card card) {
    float width = height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
    Extends ext = {
        x - width/2,
        y - height/2,
        width,
        height,
        0
    };
    OpenGL::drawImage(card.getFileName(), ext);
}

class HandNode : public LeafNode {
    private:
        std::list<Card> cards;

    public:
        Extends getCompactExtends(Extends ext) {return ext;}

        void draw() {
            drawCard(extends.x + extends.width/2 - 150, extends.y + 150, 300.0f, Card(RANK_QUEEN, SUIT_DIAMONDS));
            drawCard(extends.x + extends.width/2 - 50, extends.y + 150, 300.0f, Card(RANK_ACE, SUIT_SPADES));
            drawCard(extends.x + extends.width/2 + 50, extends.y + 150, 300.0f, Card(RANK_ACE, SUIT_HEARTS));
            drawCard(extends.x + extends.width/2 + 150, extends.y + 150, 300.0f, Card(RANK_JACK, SUIT_CLUBS));
        }

        void sendHoverEvent(float x, float y) override {
            if(!extends.contains(x,y)) return; //technically not necessary with current implementation
            std::cout << "HOVER EVENT  - x: " << x << "; y: " << y << std::endl;
        }

        void updateHand(std::list<Card> &cards) {
            sortCards(cards);
            this->cards = cards;
        }

};


GameNode::GameNode() {
    //TODO
    handNode = std::make_unique<HandNode>();
}

void GameNode::updateExtends(Extends ext) {
    handNode->updateExtends(ext);
    //TODO
}

void GameNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    //TODO
    function(handNode);
}

Extends GameNode::getCompactExtends(Extends ext) {
    return ext;
}

void GameNode::handleCardUpdate(CardUpdate update) {
    //TODO
    cast(HandNode, handNode)->updateHand(update.hand);
}

void GameNode::handleBattleStateUpdate(BattleStateUpdate update) {
    //TODO
}

void GameNode::handleAvailableActionUpdate(AvailableActionUpdate update) {
    //TODO
}

void GameNode::playerUpdateNotify() {
    //TODO
    GlobalState::players;
}