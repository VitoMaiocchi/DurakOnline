#include "game_node.hpp"
#include "master_node.hpp"
#include <iostream>
#include <unordered_set>
#include <optional>
#include "viewport.hpp"

#define HOVER_OFFSET_FACTOR 0.08f
#define CARD_DELTA_FACTOR 0.7f
#define CARD_OFFSET_FACTOR 0.2f
//TODO 
//da chunt alles ine wo grendered wird während es game lauft
//endscreen und so nöd nur das mit de charte i de mitti und so

void sortCards(std::list<Card> &cards) {
    //TODO
    //da muss mer wüsse was trumpf isch und so
}


class HandNode : public LeafNode {
    private:
        std::list<Card> cards;
        std::unordered_set<Card> selected;
        std::optional<Card> hover;

        bool computeDelta(float &delta, float &card_width, uint &N) {
            N = cards.size();
            if(N == 0) return false;

            card_width = extends.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;

            if(extends.width < card_width) {
                //degenerate case 
                //weiss nonig genau wi mer das vernünftig handled
                return false;
            }

            delta = (extends.width-card_width)/(N-1);
            if(delta > card_width * CARD_DELTA_FACTOR)  delta = card_width * CARD_DELTA_FACTOR;
            return true;
        }

        std::optional<Card> getCard(float x, float y) {
            float card_width, delta;
            uint N;
            if(!computeDelta(delta, card_width, N)) return std::nullopt;

            const float x_border = (extends.width - (card_width + delta * (N-1)))/2;
            float card_x = extends.x + x_border;

            if(x < extends.x + x_border|| x > extends.x + extends.width - x_border) return std::nullopt;
            if(y < extends.y || y > extends.y + (1+HOVER_OFFSET_FACTOR)*extends.height) return std::nullopt;

            auto it = cards.begin();
            while(it != cards.end()) {
                if(x >= card_x && x <= card_x + delta) {
                    if(*it == hover) return *it;
                    if(y > extends.y + extends.height) return std::nullopt;
                    else return *it;
                }
                card_x += delta;
                it++;
            }
            it--;
            if(*it == hover) return *it;
            if(y > extends.y + extends.height) return std::nullopt;
            else return *it;
        }

    public:
        Extends getCompactExtends(Extends ext) {return ext;}

        void draw() {
            float card_width, delta;
            uint N;
            if(!computeDelta(delta, card_width, N)) return;

            Extends image_ext = {
                extends.x + (extends.width - (card_width + delta * (N-1)) )/2,
                extends.y,
                card_width,
                extends.height,
                0
            };

            for(Card card : cards) {
                if(card == hover) image_ext.y += HOVER_OFFSET_FACTOR*extends.height;
                OpenGL::drawImage(card.getFileName(), image_ext);
                if(selected.find(card) != selected.end()) 
                    OpenGL::drawImage(std::string(CLIENT_RES_DIR) + "/cards/outline.png", image_ext);
                if(card == hover) image_ext.y = extends.y;
                image_ext.x += delta;
            }
        }

        void sendHoverEvent(float x, float y) override {
            hover = getCard(x,y);
        }

        void sendClickEvent(float x, float y) override {
            auto card = getCard(x,y);
            if(card.has_value()) {
                if(selected.find(card.value()) == selected.end()) selected.insert(card.value());
                else selected.erase(card.value());
            }
        }

        void updateHand(std::list<Card> &cards) {
            sortCards(cards);
            this->cards = cards;
        }

};

//TODO: das schönner mache
Extends computeCompactExtends(Extends ext, float height, float width);

class CardStackNode : public LeafNode {
    private:
        std::optional<Card> bottom_card;
        std::optional<Card> top_card;
    public:
        void setCard(bool top, Card card) {
            if(top) top_card = card;
            else bottom_card = card;
        }

        Extends getCompactExtends(Extends ext) {
            constexpr float h = (float)CARD_TEXTURE_HEIGHT/(1-CARD_OFFSET_FACTOR);
            constexpr float w = CARD_TEXTURE_WIDTH  + h*CARD_OFFSET_FACTOR;
            return computeCompactExtends(ext, h, w);
        }

        void draw() {
            OpenGL::drawRectangle(extends, glm::vec4(0,1,0,1));

            if(top_card.has_value() && !bottom_card.has_value()) {
                //print warning oder so
                return;
            }

            const float offset = extends.height*CARD_OFFSET_FACTOR;

            if(bottom_card.has_value() && !top_card.has_value()) {
                OpenGL::drawImage(bottom_card.value().getFileName(), {
                    extends.x + 0.5f*offset,
                    extends.y + 0.5f*offset,
                    extends.width - offset,
                    extends.height - offset,
                    0
                });
                return;
            }

            if(bottom_card.has_value()) OpenGL::drawImage(bottom_card.value().getFileName(), {
                extends.x + offset,
                extends.y,
                extends.width - offset,
                extends.height - offset,
                0
            });

            if(top_card.has_value()) OpenGL::drawImage(top_card.value().getFileName(), {
                extends.x,
                extends.y + offset,
                extends.width - offset,
                extends.height - offset,
                0
            });
        }
};

class MiddleNode : public TreeNode {
    private:
        std::unique_ptr<Node> stack; 
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
            function(stack);
        }

    public:
        MiddleNode() {
            stack = std::make_unique<LinearStackNode>();
            cast(LinearStackNode, stack)->setStackType(STACKDIRECTION_VERTICAL, STACKTYPE_COMPACT);
            auto &c = cast(LinearStackNode, stack) -> children;
            c.resize(2);
            for(uint i = 0; i < 2; i++) {
                c[i] = std::make_unique<LinearStackNode>();
                cast(LinearStackNode, c[i])->setStackType(STACKDIRECTION_HORIZONTAL, STACKTYPE_COMPACT);
                auto &c2 = cast(LinearStackNode, c[i])->children;
                c2.resize(3);
                for(uint j = 0; j < 3; j++) {
                    c2[j] = std::make_unique<CardStackNode>();
                    cast(CardStackNode, c2[j])->setCard(false, Card(RANK_ACE, SUIT_SPADES));
                    cast(CardStackNode, c2[j])->setCard(true, Card(RANK_ACE, SUIT_HEARTS));
                }
            }
        }

        void updateExtends(Extends ext) {
            stack->updateExtends(ext);
        }

        Extends getCompactExtends(Extends ext) {
            return stack->getCompactExtends(ext);
        }
};


GameNode::GameNode() {
    handNode = std::make_unique<HandNode>();
    middleNode = std::make_unique<MiddleNode>();
    //TODO
}

void GameNode::updateExtends(Extends ext) {
    extends = ext; //TODO so mache das mer das nöd immer selber mache muess

    Extends hand_ext = {
        extends.x + extends.width / 4,
        extends.y,
        extends.width / 2,
        extends.height / 3,
        0
    };
    handNode->updateExtends(hand_ext);

    Extends middle_ext = {
        extends.x + extends.width / 4,
        extends.y + extends.height * 3.0f / 8.0f,
        extends.width / 2,
        extends.height / 2,
        0
    };
    middleNode->updateExtends(middle_ext);
    //TODO
}

void GameNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    //TODO
    function(handNode);
    function(middleNode);
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