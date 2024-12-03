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

#define BUTTON_BUFFER 10
#define DECK_BUFFER 10
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
                extends.height
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

class CardStackNode : public LeafNode {
    private:
        std::optional<Card> bottom_card;
        std::optional<Card> top_card;
    public:
        void setCard(bool top, std::optional<Card> card) {
            if(top) top_card = card;
            else bottom_card = card;
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
                    ext.height - offset
                }, glm::vec4(0.4,0.2,0.2,0.2));
            }

            if(bottom_card.has_value() && !top_card.has_value()) {
                OpenGL::drawImage(bottom_card.value().getFileName(), {
                    ext.x + 0.5f*offset,
                    ext.y + 0.5f*offset,
                    ext.width - offset,
                    ext.height - offset
                });
                return;
            }

            if(bottom_card.has_value()) OpenGL::drawImage(bottom_card.value().getFileName(), {
                ext.x,
                ext.y + offset,
                ext.width - offset,
                ext.height - offset
            });

            if(top_card.has_value()) OpenGL::drawImage(top_card.value().getFileName(), {
                ext.x + offset,
                ext.y,
                ext.width - offset,
                ext.height - offset
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

class PlayerNode : public LeafNode {

    const Player* player;

    public:
    PlayerNode(const Player* player) : player(player) {}

    Extends getCompactExtends(Extends ext) {
        if(ext.height >  ext.width) {
            ext.y += (ext.height-ext.width)/2;
            ext.height = ext.width;
        } else {
            ext.x += (ext.width-ext.height)/2;
            ext.width = ext.height;
        }
        return ext;
    }

    void draw() { //TODO: mache das di zahle schön aligned sind
        if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,0.1));

        auto size = OpenGL::getImageDimensions(CLIENT_RES_DIR + "skins/durak.png");
        Extends ext = computeCompactExtends({
            extends.x,
            extends.y + extends.height * 0.3f,
            extends.width,
            extends.height * 0.7f
        }, size.second, size.first);
        OpenGL::drawImage(CLIENT_RES_DIR + "skins/durak.png", ext);

        size = OpenGL::getImageDimensions(CLIENT_RES_DIR + "icons/hand.png");
        OpenGL::drawImage(CLIENT_RES_DIR + "icons/hand.png", computeCompactExtends({
            extends.x + extends.width * 0.15f,
            extends.y + extends.height * 0.15f,
            extends.width * 0.2f,
            extends.height * 0.15f
        }, size.second, size.first));

        OpenGL::drawText(std::to_string(player->game->cards), {
            extends.x + extends.width * 0.35f,
            extends.y + extends.height * 0.15f,
            extends.width * 0.2f,
            extends.height * 0.15f
        }, glm::vec3(0,0,0), TEXTSIZE_LARGE);

        if(player->game->state != PLAYERSTATE_NONE) {
            const std::string s = getPlayerStateIcon(player->game->state);
            size = OpenGL::getImageDimensions(s);
            OpenGL::drawImage(s, computeCompactExtends({
                extends.x + extends.width * 0.65f,
                extends.y + extends.height * 0.15f,
                extends.width * 0.2f,
                extends.height * 0.15f
            }, size.second, size.first));
        }

        OpenGL::drawText(player->name, {
            extends.x,
            extends.y,
            extends.width,
            extends.height * 0.15f
        }, glm::vec3(0,0,0), TEXTSIZE_MEDIUM);
    }
};

class PlayerBarNode : public TreeNode {
    std::list<std::unique_ptr<Node>> playerNodes;

    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
        for(auto &player : playerNodes) function(player);
    }

    public:
    void playerUpdateNotify() {
        playerNodes.clear();
        for(const Player& player : GlobalState::players) 
            if(!player.is_you) playerNodes.push_back(std::make_unique<PlayerNode>(&player));
        
        updateExtends(extends);
    }

    void updateExtends(Extends ext) {
        ext = getCompactExtends(ext);
        extends = ext; //TODO: das isch hässlich

        const uint N = playerNodes.size();
        if(N == 0) return;
        if(N == 1) {
            (*playerNodes.begin())->updateExtends(ext);
            return;
        }

        const float s = ext.height;
        const float delta = (ext.width - s) / (N -1);
        float x = ext.x;
        for(auto &player : playerNodes) {
            player->updateExtends({x, ext.y, s, s});
            x += delta;
        }
    }

    Extends getCompactExtends(Extends ext) {
        const uint N = playerNodes.size();
        if(ext.width > ext.height * N) return ext;
        const float h = ext.width / N;
        return {
            ext.x,
            ext.y + (ext.height - h)/2,
            ext.width,
            h
        };
    }
};

class PlayerActionNode : public TreeNode {

    // {ok,pickup,passon}
    std::unique_ptr<Node> buttons[3];

    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
        for(auto &node : buttons) function(node);
    }

    public:
    PlayerActionNode() {
        buttons[0] = std::make_unique<ButtonNode>("Ready");
        buttons[1] = std::make_unique<ButtonNode>("Pick Up");
        buttons[2] = std::make_unique<ButtonNode>("Reflect");
        buttons[0]->setClickEventCallback([](float x, float y){
            std::cout << "CLICKED READY" << std::endl;
            ClientActionEvent event;
            event.action = CLIENTACTION_OK;
            Network::sendMessage(std::make_unique<ClientActionEvent>(event));
        });
        buttons[1]->setClickEventCallback([](float x, float y){
            std::cout << "CLICKED PICKUP" << std::endl;
            ClientActionEvent event;
            event.action = CLIENTACTION_PICK_UP;
            Network::sendMessage(std::make_unique<ClientActionEvent>(event));
        });
        buttons[2]->setClickEventCallback([](float x, float y){
            std::cout << "CLICKED REFLECT" << std::endl;
            ClientActionEvent event;
            event.action = CLIENTACTION_PASS_ON;
            Network::sendMessage(std::make_unique<ClientActionEvent>(event));
        });
    }

    void handleAvailableActionUpdate(AvailableActionUpdate update) {
        std::cout << "handle available action update" <<  update.ok << update.pick_up << std::endl; 
        cast(ButtonNode, buttons[0])->visible = update.ok;
        cast(ButtonNode, buttons[1])->visible = update.pick_up;
        cast(ButtonNode, buttons[2])->visible = update.pass_on;
    }

    void updateExtends(Extends ext) {
        extends = getCompactExtends(ext);

        float delta = extends.height / 3;
        const float b = BUTTON_BUFFER * Viewport::global_scalefactor;
        ext = {
            extends.x + b,
            extends.y + b,
            extends.width - 2*b,
            delta - 2*b
        };

        for(auto &node : buttons) {
            node->updateExtends(ext);
            ext.y += delta;
        }
    }

    Extends getCompactExtends(Extends ext) {
        return ext;
    }
};

class DeckNode : public LeafNode {
    
    Card trump_card = Card();
    Suit trump_suit = SUIT_CLUBS;
    uint draw_pile_cards = 0;

    public:
    Extends getCompactExtends(Extends ext) {
        Extends e = computeCompactExtends(ext, 1.8f, 1);
        e.x = ext.x;
        return e;
    }

    void draw() {
        if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,0.1));

        const float b = Viewport::global_scalefactor * DECK_BUFFER;
        Extends ext = {
            extends.x + b,
            extends.y + b,
            extends.width - 2*b,
            extends.height - 2*b
        };

        Extends ext2 = {
            ext.x + b,
            ext.y + b,
            ext.width - 2*b,
            ext.height - 2*b
        };

        Extends image_ext = computeCompactExtends(ext2, CARD_TEXTURE_HEIGHT, CARD_TEXTURE_WIDTH);
        image_ext.y = ext.y + b;
        const float h = image_ext.height + 2*b;
        float delta = (ext.height - h) / 2;
        Extends text_ext = {
            ext.x,
            ext.y + h,
            ext.width,
            delta
        };
        OpenGL::drawText("Trump Card", text_ext, glm::vec3(0,0,0), TEXTSIZE_MEDIUM);
        text_ext.y += delta;
        OpenGL::drawText("Cards Left: "+std::to_string(draw_pile_cards), text_ext, glm::vec3(0,0,0), TEXTSIZE_MEDIUM);

        if(draw_pile_cards != 0) {
            OpenGL::drawImage(trump_card.getFileName(), image_ext);
            return;
        }

        std::string image = "PLACEHOLDER (suit image): ";
        switch(trump_suit) {
            case SUIT_CLUBS:
                image +="clubs.png";
            break;
            case SUIT_HEARTS:
                image +="hearts.png";
            break;
            case SUIT_DIAMONDS:
                image +="diamonds.png";
            break;
            case SUIT_SPADES:
                image +="spades.png";
            break;
        }

        OpenGL::drawText(image, image_ext, glm::vec3(0.1,0.2,1), TEXTSIZE_LARGE);
    }

    void handleCardUpdate(CardUpdate update) {
        trump_card = update.trump_card;
        trump_suit = update.trump_suit;
        draw_pile_cards = update.draw_pile_cards;
    }
};

GameNode::GameNode(Extends ext) {
    handNode = std::make_unique<HandNode>();
    playerBarNode = std::make_unique<PlayerBarNode>();
    playerActionNode = std::make_unique<PlayerActionNode>();
    deckNode = std::make_unique<DeckNode>();

    middleNode = std::make_unique<MiddleNode>();
    Node* hand_ptr = handNode.get(); //only for lambda (unique pointer exception)
    for(uint s = CARDSLOT_1; s != CARDSLOT_1_TOP; s++) {
        const CardSlot slot = (CardSlot) s;
        cast(MiddleNode, middleNode)->setStackClickCallback(slot, [slot, hand_ptr](float x, float y){
            std::unordered_set<Card> cards = dynamic_cast<HandNode*>(hand_ptr)->resetSelected();
            PlayCardEvent event;
            event.cards = cards;
            event.slot = slot;
            Network::sendMessage(std::make_unique<PlayCardEvent>(event));
        });
    }
    
    playerUpdateNotify();
    updateExtends(ext);
}

void GameNode::updateExtends(Extends ext) {
    extends = ext;

    Extends hand_ext = {
        extends.x + extends.width / 4,
        extends.y,
        extends.width / 2,
        extends.height * 0.2f
    };
    handNode->updateExtends(hand_ext);

    Extends playerbar_ext {
        extends.x,
        extends.y + extends.height * 0.775f,
        extends.width,
        extends.height * 0.2f
    };
    playerBarNode->updateExtends(playerbar_ext);

    Extends middle_ext = {
        extends.x + extends.width * 0.1f,
        extends.y + extends.height * 0.25f,
        extends.width * 0.8f,
        extends.height * 0.5f
    };
    middleNode->updateExtends(middle_ext);

    playerActionNode->updateExtends({
        extends.x + extends.width * 0.75f,
        extends.y,
        extends.width * 0.25f,
        extends.height * 0.2f
    });

    deckNode->updateExtends({
        extends.x,
        extends.y,
        extends.width * 0.25f,
        extends.height * 0.25f
    });
}

void GameNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(handNode);
    function(middleNode);
    function(playerBarNode);
    function(playerActionNode);
    function(deckNode);
}

Extends GameNode::getCompactExtends(Extends ext) {
    return ext;
}

void GameNode::handleCardUpdate(CardUpdate update) {
    cast(HandNode, handNode)->updateHand(update.hand);
    cast(MiddleNode, middleNode)->setCards(update.middle_cards);
    cast(DeckNode, deckNode)->handleCardUpdate(update);

    for(auto entry : update.opponent_cards) {
        auto it = GlobalState::players.find({entry.first});
        throwServerErrorIF("invalid ClientID in opponent cards entry", it == GlobalState::players.end());
        it->game->cards = entry.second;
    }
}

void GameNode::handleBattleStateUpdate(BattleStateUpdate update) {
    for(ClientID id : update.attackers) {
        auto it = GlobalState::players.find({id});
        throwServerErrorIF("trying to assign attacking state to nonexistent player", it == GlobalState::players.end());
        it->game->state = PLAYERSTATE_ATTACK;
    }

    for(ClientID id : update.idle) {
        auto it = GlobalState::players.find({id});
        throwServerErrorIF("trying to assign idle battle state to nonexistent player", it == GlobalState::players.end());
        it->game->state = PLAYERSTATE_IDLE;
    }

    auto it = GlobalState::players.find({update.defender});
    throwServerErrorIF("trying to assign defending state to nonexistent player", it == GlobalState::players.end());
    it->game->state = PLAYERSTATE_DEFEND;
}

void GameNode::handleAvailableActionUpdate(AvailableActionUpdate update) {
    cast(PlayerActionNode, playerActionNode)->handleAvailableActionUpdate(update);
}

void GameNode::playerUpdateNotify() {
    cast(PlayerBarNode, playerBarNode)->playerUpdateNotify();
}