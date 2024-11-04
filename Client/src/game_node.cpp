#include "game_node.hpp"
#include "master_node.hpp"
#include <iostream>
#include <unordered_set>
#include <optional>

//TODO 
//da chunt alles ine wo grendered wird während es game lauft
//endscreen und so nöd nur das mit de charte i de mitti und so

void sortCards(std::list<Card> &cards) {
    //TODO
    //da muss mer wüsse was trumpf isch und so
}

//das sött no relativ zum ext si
#define HOVER_OFFSET 70

class HandNode : public LeafNode {
    private:
        std::list<Card> cards;
        std::unordered_set<Card> selected;
        std::optional<Card> hover;

    public:
        Extends getCompactExtends(Extends ext) {return ext;}

        void draw() {
            uint N = cards.size();
            if(N == 0) return;

            const float card_width = extends.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
            if(extends.width < card_width) {
                //degenerate case 
                //TODO: compact extends für master node und so 
                return;
            }

            Extends image_ext = {
                extends.x,
                extends.y,
                card_width,
                extends.height,
                0
            };

            float delta = (extends.width-card_width)/(N-1);
            if(delta > card_width * 0.7) {
                delta = card_width * 0.7;
                const double w = card_width + delta * (N-1);
                image_ext.x += (extends.width - w)/2;
            }
            for(Card card : cards) {
                if(card == hover) image_ext.y += HOVER_OFFSET;
                OpenGL::drawImage(card.getFileName(), image_ext);
                if(selected.find(card) != selected.end()) 
                    OpenGL::drawImage(std::string(CLIENT_RES_DIR) + "/cards/outline.png", image_ext);
                if(card == hover) image_ext.y = extends.y;
                image_ext.x += delta;
            }
        }

        void sendHoverEvent(float x, float y) override {

            uint N = cards.size();
            if(N == 0) return;

            const float card_width = extends.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
            if(extends.width < card_width) {
                //degenerate case 
                //TODO: compact extends für master node und so 
                return;
            }

            float card_x = extends.x;

            float delta = (extends.width-card_width)/(N-1);
            if(delta > card_width * 0.7) {
                delta = card_width * 0.7;
                const double w = card_width + delta * (N-1);
                card_x += (extends.width - w)/2;
            }

            if(x < extends.x) {
                hover.reset();
                return;
            }
            if(x > extends.x + extends.width) {
                hover.reset();
                return;
            }
            auto it = cards.begin();
            while(it != cards.end()) {
                if(x > card_x && x < card_x + delta) {
                    if(*it == hover && y > extends.height + HOVER_OFFSET) {
                        hover.reset();
                        return;
                    }
                    if(!(*it == hover) && y > extends.height) {
                        hover.reset();
                        return;
                    }
                    if(!(*it == hover) && y < extends.height) {
                        hover = *it;
                        return;
                    }
                }
                card_x += delta;
                it++;
            }
            it--;
            if(x > card_x && x < card_x + card_width) {
                if(*it == hover && y > extends.height + HOVER_OFFSET) {
                    hover.reset();
                    return;
                }
                if(!(*it == hover) && y > extends.height) {
                        hover.reset();
                        return;
                    }
                if(!(*it == hover) && y < extends.height) {
                    hover = *it;
                    return;
                }
            }
        }

        void toggleSelect(Card card) {
            if(selected.find(card) == selected.end()) selected.insert(card);
            else selected.erase(card);
        }

        void sendClickEvent(float x, float y) override {

            uint N = cards.size();
            if(N == 0) return;

            const float card_width = extends.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
            if(extends.width < card_width) {
                //degenerate case 
                //TODO: compact extends für master node und so 
                return;
            }

            float card_x = extends.x;

            float delta = (extends.width-card_width)/(N-1);
            if(delta > card_width * 0.7) {
                delta = card_width * 0.7;
                const double w = card_width + delta * (N-1);
                card_x += (extends.width - w)/2;
            }

            if(x < extends.x) {
                return;
            }
            if(x > extends.x + extends.width) {
                return;
            }
            auto it = cards.begin();
            while(it != cards.end()) {
                if(x > card_x && x < card_x + delta) {
                    if(*it == hover && y < extends.height + HOVER_OFFSET) {
                        toggleSelect(*it);
                        return;
                    }
                    if(!(*it == hover) && y < extends.height) {
                        toggleSelect(*it);
                        return;
                    }
                }
                card_x += delta;
                it++;
            }
            it--;
            if(x > card_x && x < card_x + card_width) {
                if(*it == hover && y < extends.height + HOVER_OFFSET) {
                    toggleSelect(*it);
                    return;
                }
                if(!(*it == hover) && y < extends.height) {
                    toggleSelect(*it);
                    return;
                }
            }
        }

        void updateHand(std::list<Card> &cards) {
            sortCards(cards);
            this->cards = cards;
            //selected.insert(Card(RANK_KING, SUIT_DIAMONDS));
        }

};


GameNode::GameNode() {
    //TODO
    handNode = std::make_unique<HandNode>();
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