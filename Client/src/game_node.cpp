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

#define HOVER_OFFSET 70

void drawCards(Extends ext, std::list<Card> cards, std::unordered_set<Card> &selected, std::optional<Card> &hover) {
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
        if(card == hover) image_ext.y += HOVER_OFFSET;
        OpenGL::drawImage(card.getFileName(), image_ext);
        if(selected.find(card) != selected.end()) 
            OpenGL::drawImage(std::string(CLIENT_RES_DIR) + "/cards/outline.png", image_ext);
        if(card == hover) image_ext.y = ext.y;
        image_ext.x += delta;
    }
}

class HandNode : public LeafNode {
    private:
        std::list<Card> cards;
        std::unordered_set<Card> selected;
        std::optional<Card> hover;

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
            drawCards(ext, cards, selected, hover);
        }

        void sendHoverEvent(float x, float y) override {
            //std::cout << "HOVER EVENT  - x: " << x << "; y: " << y << std::endl;
            if(!extends.contains(x,y)) return; //technically not necessary with current implementation

            Extends hand_ext = {
                extends.x + extends.width / 4,
                extends.y,
                extends.width / 2,
                extends.height / 3,
                0
            };

            uint N = cards.size();
            if(N == 0) return;

            const float card_width = hand_ext.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
            if(hand_ext.width < card_width) {
                //degenerate case 
                //TODO: compact extends für master node und so 
                return;
            }

            float card_x = hand_ext.x;

            float delta = (hand_ext.width-card_width)/(N-1);
            if(delta > card_width * 0.7) {
                delta = card_width * 0.7;
                const double w = card_width + delta * (N-1);
                card_x += (hand_ext.width - w)/2;
            }

            if(x < hand_ext.x) {
                hover.reset();
                return;
            }
            if(x > hand_ext.x + hand_ext.width) {
                hover.reset();
                return;
            }
            auto it = cards.begin();
            while(it != cards.end()) {
                if(x > card_x && x < card_x + delta) {
                    if(*it == hover && y > hand_ext.height + HOVER_OFFSET) {
                        hover.reset();
                        return;
                    }
                    if(!(*it == hover) && y < hand_ext.height) {
                        hover = *it;
                        return;
                    }
                }
                card_x += delta;
                it++;
            }
            it--;
            if(x > card_x && x < card_x + card_width) {
                if(*it == hover && y > hand_ext.height + HOVER_OFFSET) {
                    hover.reset();
                    return;
                }
                if(!(*it == hover) && y < hand_ext.height) {
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
            if(!extends.contains(x,y)) return; //technically not necessary with current implementation

            Extends hand_ext = {
                extends.x + extends.width / 4,
                extends.y,
                extends.width / 2,
                extends.height / 3,
                0
            };

            uint N = cards.size();
            if(N == 0) return;

            const float card_width = hand_ext.height / CARD_TEXTURE_HEIGHT * CARD_TEXTURE_WIDTH;
            if(hand_ext.width < card_width) {
                //degenerate case 
                //TODO: compact extends für master node und so 
                return;
            }

            float card_x = hand_ext.x;

            float delta = (hand_ext.width-card_width)/(N-1);
            if(delta > card_width * 0.7) {
                delta = card_width * 0.7;
                const double w = card_width + delta * (N-1);
                card_x += (hand_ext.width - w)/2;
            }

            if(x < hand_ext.x) {
                return;
            }
            if(x > hand_ext.x + hand_ext.width) {
                return;
            }
            auto it = cards.begin();
            while(it != cards.end()) {
                if(x > card_x && x < card_x + delta) {
                    if(*it == hover && y < hand_ext.height + HOVER_OFFSET) {
                        toggleSelect(*it);
                        return;
                    }
                    if(!(*it == hover) && y < hand_ext.height) {
                        toggleSelect(*it);
                        return;
                    }
                }
                card_x += delta;
                it++;
            }
            it--;
            if(x > card_x && x < card_x + card_width) {
                if(*it == hover && y < hand_ext.height + HOVER_OFFSET) {
                    toggleSelect(*it);
                    return;
                }
                if(!(*it == hover) && y < hand_ext.height) {
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