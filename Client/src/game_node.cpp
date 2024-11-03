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


void drawCards(Extends ext, std::list<Card> cards) {
    uint N = cards.size();
    if(N == 0) return;

    const float card_width = ext.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
    if(ext.width < card_width) {
        //degenerate case 
        //TODO: compact extends für master node und so 
        return;
    }

    Extends image_ext = {
        ext.x,
        ext.y,
        card_width,
        ext.height,
        0
    };

    float delta = (ext.width-card_width)/(N-1);
    if(delta > card_width * 0.7) {
        delta = card_width * 0.7;
        const double w = card_width + delta * (N-1);
        image_ext.x += (ext.width - w)/2;
    }
    for(Card card : cards) {
        OpenGL::drawImage(card.getFileName(), image_ext);
        image_ext.x += delta;
    }
}

class HandNode : public LeafNode {
    private:
        std::list<Card> cards;

    public:
        Extends getCompactExtends(Extends ext) {return ext;}

        void draw() {
            Extends ext = {
                extends.x + extends.width / 4,
                extends.y,
                extends.width / 2,
                extends.height / 3,
                0
            };
            drawCards(ext, cards);
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