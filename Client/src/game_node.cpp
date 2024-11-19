#include "game_node.hpp"
#include "master_node.hpp"
#include <iostream>
#include <unordered_set>
#include <optional>
#include "viewport.hpp"

#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>

#define HOVER_OFFSET_FACTOR 0.08f
#define CARD_DELTA_FACTOR 0.7f

#define CARD_OFFSET_FACTOR 0.11f
#define CARD_OFFSET_BORDER 7
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

        std::unordered_set<Card> resetSelected() {
            auto ret = selected;
            selected = {};
            return ret;
        }

};

//TODO: das schönner mache
Extends computeCompactExtends(Extends ext, float height, float width);

class CardStackNode : public LeafNode {
    private:
        std::optional<Card> bottom_card;
        std::optional<Card> top_card;
        bool hover = false;
    public:
        void setCard(bool top, std::optional<Card> card) {
            if(top) top_card = card;
            else bottom_card = card;
        }

        void sendHoverEvent(float x, float y) override {
            if(extends.contains(x,y)) hover = true;
            else hover = false;
        }

        Extends getCompactExtends(Extends ext) {
            constexpr float h = (float)CARD_TEXTURE_HEIGHT/(1-CARD_OFFSET_FACTOR);
            constexpr float w = CARD_TEXTURE_WIDTH  + h*CARD_OFFSET_FACTOR;
            ext = {
                ext.x      +   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.y      +   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.width  - 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.height - 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor
            };
            ext = computeCompactExtends(ext, h, w);
            ext = {
                ext.x      -   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.y      -   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.width  + 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                ext.height + 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor
            };
            return ext;
        }

        void draw() {
            if(hover) OpenGL::drawRectangle(extends, glm::vec4(0.4,0.2,0.2,0.4));
            else OpenGL::drawRectangle(extends, glm::vec4(0.4,0.2,0.2,0.2));

            Extends ext = {
                extends.x      +   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                extends.y      +   CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                extends.width  - 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor,
                extends.height - 2*CARD_OFFSET_BORDER*Viewport::global_scalefactor
            };

            if(top_card.has_value() && !bottom_card.has_value()) {
                //print warning oder so
                return;
            }

            const float offset = ext.height*CARD_OFFSET_FACTOR;

            if(!top_card.has_value() && !bottom_card.has_value()) {
                OpenGL::drawRectangle({
                    ext.x + 0.5f*offset,
                    ext.y + 0.5f*offset,
                    ext.width - offset,
                    ext.height - offset,
                    0
                }, glm::vec4(0.4,0.2,0.2,0.2));
            }

            if(bottom_card.has_value() && !top_card.has_value()) {
                OpenGL::drawImage(bottom_card.value().getFileName(), {
                    ext.x + 0.5f*offset,
                    ext.y + 0.5f*offset,
                    ext.width - offset,
                    ext.height - offset,
                    0
                });
                return;
            }

            if(bottom_card.has_value()) OpenGL::drawImage(bottom_card.value().getFileName(), {
                ext.x,
                ext.y + offset,
                ext.width - offset,
                ext.height - offset,
                0
            });

            if(top_card.has_value()) OpenGL::drawImage(top_card.value().getFileName(), {
                ext.x + offset,
                ext.y,
                ext.width - offset,
                ext.height - offset,
                0
            });
        }
};

#define MAX_FLOAT 10E10 //todo

class MiddleNode : public TreeNode {
    private:
        std::unique_ptr<Node> cardStacks[6];

        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
            for(auto &stack : cardStacks) function(stack);
        }

    public:
        MiddleNode() {
            for(auto &stack : cardStacks) stack = std::make_unique<CardStackNode>();
        }

        void updateExtends(Extends ext) {
            extends = getCompactExtends(ext);
            const float w = extends.width / 3;
            const float h = extends.height / 2;
            for(uint i = 0; i<3; i++) {
                cardStacks[i+3]->updateExtends({extends.x+i*w, extends.y + h, w, h});
                cardStacks[i]->updateExtends({extends.x+i*w, extends.y, w, h});
            }
        }

        Extends getCompactExtends(Extends ext) {
            float width = 3*cardStacks[0]->getCompactExtends({0,0, MAX_FLOAT, ext.height/2}).width;
            float height = 2*cardStacks[0]->getCompactExtends({0,0, ext.width/3, MAX_FLOAT}).height;
            if(width > ext.width) width = ext.width;
            else height = ext.height;
            return {
                ext.x + (ext.width - width)/2,
                ext.y + (ext.height - height)/2,
                width,
                height
            };
        }

        void sendHoverEvent(float x, float y) override {
            for(auto &stack : cardStacks) stack->sendHoverEvent(x,y);
        }

        void setCards(std::map<CardSlot, Card> &cards) {
            for(uint slot = 0; slot < (uint)CARDSLOT_NONE; slot++) {
                uint stack = slot % 6;
                bool top = slot/6;
                std::optional<Card> card;
                if(cards.find((CardSlot)slot) != cards.end()) card = cards[(CardSlot)slot];
                cast(CardStackNode, cardStacks[stack])->setCard(top, card);
            }
        }

        void setStackClickCallback(CardSlot slot, std::function<void(float, float)> callback) {
            assert(slot < CARDSLOT_1_TOP);
            cardStacks[slot]->setClickEventCallback(callback);
        }
};


GameNode::GameNode() {
    handNode = std::make_unique<HandNode>();
    middleNode = std::make_unique<MiddleNode>();
    Node* hand_ptr = handNode.get(); //only for lambda (unique pointer exception)
    for(uint s = CARDSLOT_1; s != CARDSLOT_1_TOP; s++) {
        const CardSlot slot = (CardSlot) s;
        cast(MiddleNode, middleNode)->setStackClickCallback(slot, [slot, hand_ptr](float x, float y){
            std::unordered_set<Card> cards = dynamic_cast<HandNode*>(hand_ptr)->resetSelected();
            PlayCardEvent event;
            event.cards = cards;
            event.slot = slot;
            std::cout << "SEND EVENT: \n" << event.toJson() << std::endl; //DEBUG
            //Network::sendMessage(std::make_unique<PlayCardEvent>(event));
        });
    }
    
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
    cast(MiddleNode, middleNode)->setCards(update.middle_cards);
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