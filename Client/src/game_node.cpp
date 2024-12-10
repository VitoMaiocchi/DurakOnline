#include "game_node.hpp"
#include "global_state.hpp"

#include <iostream>
#include <unordered_set>
#include <optional>
#include <algorithm>
#include <vector>

#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>

#define HOVER_OFFSET_FACTOR 0.08f
#define CARD_DELTA_FACTOR 0.7f

#define CARD_OFFSET_FACTOR 0.11f
#define CARD_OFFSET_BORDER 7

#define BUTTON_BUFFER 10
#define DECK_BUFFER 10

Suit GlobalState::trump_suit = SUIT_HEARTS;

void sortCards(std::vector<Card> &cards) {
    std::sort(cards.begin(), cards.end(), [](const Card &a, const Card &b){
        if(a.suit == GlobalState::trump_suit && b.suit != GlobalState::trump_suit) return false;
        if(a.suit != GlobalState::trump_suit && b.suit == GlobalState::trump_suit) return true;
        if(a.rank == b.rank) return a.suit > b.suit; 
        return a.rank > b.rank;
    });
}


class HandNode : public LeafNode {
    private:
        std::vector<Card> cards;
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
        Extends getCompactExtends(Extends ext) override {return ext;}

        void draw() override {
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

        void updateHand(std::vector<Card> &cards) {
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

        Extends getCompactExtends(Extends ext) override {
            constexpr float h = (float)CARD_TEXTURE_HEIGHT/(1-CARD_OFFSET_FACTOR);
            constexpr float w = CARD_TEXTURE_WIDTH  + h*CARD_OFFSET_FACTOR;
            ext = applyBorder(ext, CARD_OFFSET_BORDER*Viewport::global_scalefactor);
            ext = computeCompactExtends(ext, h, w);
            ext = applyBorder(ext, -CARD_OFFSET_BORDER*Viewport::global_scalefactor);
            return ext;
        }

        void draw() override {
            if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,2*DEFAULT_TRANSPARANCY));
            else OpenGL::drawRectangle(extends, glm::vec4(0,0,0,DEFAULT_TRANSPARANCY));

            Extends ext = applyBorder(extends, CARD_OFFSET_BORDER*Viewport::global_scalefactor);

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
                }, glm::vec4(0,0,0,DEFAULT_TRANSPARANCY));
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

        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) override {
            for(auto &stack : cardStacks) function(stack);
        }

    public:
        MiddleNode() {
            for(auto &stack : cardStacks) stack = std::make_unique<CardStackNode>();
        }

        void updateExtends(Extends ext) override {
            extends = getCompactExtends(ext);
            const float w = extends.width / 3;
            const float h = extends.height / 2;
            for(uint i = 0; i<3; i++) {
                cardStacks[i+3]->updateExtends({extends.x+i*w, extends.y + h, w, h});
                cardStacks[i]->updateExtends({extends.x+i*w, extends.y, w, h});
            }
        }

        Extends getCompactExtends(Extends ext) override {
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

class PlayerBarNode : public TreeNode {
    std::list<std::unique_ptr<Node>> playerNodes;

    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) override {
        for(auto &player : playerNodes) function(player);
    }

    public:
    void playerUpdateNotify() {
        playerNodes.clear();
        if(GlobalState::players.size() == 0) return;

        auto you_it = GlobalState::players.find({GlobalState::clientID});
        throwServerErrorIF("This client ClientID is not part of the player update", you_it == GlobalState::players.end());

        auto it = you_it;
        it++;
        while(it != GlobalState::players.end()) {
            playerNodes.push_front(std::make_unique<PlayerNode>(&(*it), true));
            it++;
        }
        it = GlobalState::players.begin();
        while(it != you_it) {
            playerNodes.push_front(std::make_unique<PlayerNode>(&(*it), true));
            it++;
        }

        updateExtends(extends);
    }

    void updateExtends(Extends ext) override {
        extends = getCompactExtends(ext);

        const uint N = playerNodes.size();
        if(N == 0) return;
        if(N == 1) {
            (*playerNodes.begin())->updateExtends(extends);
            return;
        }

        const float s = extends.height;
        const float delta = (extends.width - s) / (N -1);
        float x = extends.x;
        for(auto &player : playerNodes) {
            player->updateExtends({x, extends.y, s, s});
            x += delta;
        }
    }

    Extends getCompactExtends(Extends ext) override {
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

    void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) override {
        for(auto &node : buttons) function(node);
    }

    public:
    PlayerActionNode() {
        buttons[0] = std::make_unique<ButtonNode>("Done");
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

    void updateExtends(Extends ext) override {
        extends = getCompactExtends(ext);

        float delta = extends.height / 3;
        ext.height = delta;
        ext = applyBorder(ext, BUTTON_BUFFER * Viewport::global_scalefactor);

        for(auto &node : buttons) {
            node->updateExtends(ext);
            ext.y += delta;
        }
    }

    Extends getCompactExtends(Extends ext) override {
        const float max_w = ext.height * 1.4f;
        if(ext.width < max_w) return ext;

        ext.x = ext.x + ext.width - max_w;
        ext.width = max_w;
        return ext;
    }
};

class DeckNode : public LeafNode {
    
    Card trump_card = Card();
    Suit trump_suit = SUIT_CLUBS;
    uint draw_pile_cards = 0;

    public:
    Extends getCompactExtends(Extends ext) override {
        Extends e = computeCompactExtends(ext, 1.8f, 1);
        e.x = ext.x;
        return e;
    }

    void draw() override {
        if(hover) OpenGL::drawRectangle(extends, glm::vec4(0,0,0,DEFAULT_TRANSPARANCY));

        const float b = Viewport::global_scalefactor * DECK_BUFFER;
        Extends ext = applyBorder(extends, b);
        Extends image_ext = computeCompactExtends(applyBorder(ext, b), CARD_TEXTURE_HEIGHT, CARD_TEXTURE_WIDTH);
        image_ext.y = ext.y + b;
        const float h = image_ext.height + 2*b;
        float delta = (ext.height - h) / 2;
        Extends text_ext = {
            ext.x,
            ext.y + h,
            ext.width,
            delta
        };
        OpenGL::drawText("Trump Card", text_ext, COLOR_BLACK, TEXTSIZE_MEDIUM);
        text_ext.y += delta;
        OpenGL::drawText("Cards Left: "+std::to_string(draw_pile_cards), text_ext, COLOR_BLACK, TEXTSIZE_MEDIUM);

        if(draw_pile_cards != 0) {
            OpenGL::drawImage(trump_card.getFileName(), image_ext);
            return;
        }

        std::string image = CLIENT_RES_DIR + "cards/trump_of_";
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
            default:
                throwServerError("Invalid Suit");
        }

        OpenGL::drawImage(image, image_ext);
    }

    void handleCardUpdate(CardUpdate update) {
        trump_card = update.trump_card;
        trump_suit = update.trump_suit;
        draw_pile_cards = update.draw_pile_cards;
    }
};

class PlayerStateNode : public LeafNode {

    public:
    Extends getCompactExtends(Extends ext) override {
        return ext;
    }

    void draw() override {
        const auto state = GlobalState::players.find({GlobalState::clientID})->game->state;
        if(state == PLAYERSTATE_NONE) return;

        const std::string path = getPlayerStateIcon(state);
        std::string text = " You are currently ";
        switch(state) {
            case PLAYERSTATE_ATTACK:
            text += "attacking";
            break;
            case PLAYERSTATE_DEFEND:
            text += "defending";
            break;
            case PLAYERSTATE_IDLE:
            text += "watching";
            break;
            default:
            throwServerError("Invalid PlayerState");
        }

        auto tsize = OpenGL::getTextDimensions(text, TEXTSIZE_MEDIUM);
        auto isize = OpenGL::getImageDimensions(path);
        isize.second = extends.height / 2.0f * isize.second / isize.first;
        isize.first = extends.height / 2.0f;

        float w = tsize.first + isize.first;
        float x = extends.x + (extends.width - w)/2;

        Extends image_ext = {
            x,
            extends.y + extends.height * 0.25f,
            (float) isize.first,
            (float) isize.second
        };

        Extends text_ext = {
            x + isize.first,
            extends.y,
            tsize.first,
            extends.height
        };

        OpenGL::drawText(text, text_ext, COLOR_BLACK, TEXTSIZE_MEDIUM);
        OpenGL::drawImage(path, image_ext);
    }


};

GameNode::GameNode(Extends ext) {
    handNode = std::make_unique<HandNode>();
    playerBarNode = std::make_unique<PlayerBarNode>();
    playerActionNode = std::make_unique<PlayerActionNode>();
    deckNode = std::make_unique<DeckNode>();
    playerStateNode = std::make_unique<PlayerStateNode>();

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

    handNode->updateExtends(alignExtends(extends, 0.25f, -0.2f*CARD_OFFSET_FACTOR, 0.5f, 0.2f));
    playerBarNode->updateExtends(alignExtends(extends, 0, 0.775f, 1, 0.2f));
    middleNode->updateExtends(alignExtends(extends, 0.1f, 0.25f, 0.8f, 0.5f));
    playerActionNode->updateExtends(alignExtends(extends, 0.75f, 0, 0.25f, 0.2f));
    deckNode->updateExtends(alignExtends(extends, 0,0,0.25f,0.25f));
    playerStateNode->updateExtends(alignExtends(extends, 0.3f, 0.2f, 0.4f, 0.05f));
}

void GameNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(handNode);
    function(middleNode);
    function(playerBarNode);
    function(playerActionNode);
    function(deckNode);
    function(playerStateNode);
}

Extends GameNode::getCompactExtends(Extends ext) {
    return ext;
}

void GameNode::handleCardUpdate(CardUpdate update) {
    GlobalState::trump_suit = update.trump_suit;
    auto cards = std::vector<Card>(update.hand.begin(), update.hand.end());
    cast(HandNode, handNode)->updateHand(cards);
    cast(MiddleNode, middleNode)->setCards(update.middle_cards);
    cast(DeckNode, deckNode)->handleCardUpdate(update);

    for(auto entry : update.opponent_cards) {
        auto it = GlobalState::players.find({entry.first});
        throwServerErrorIF("invalid ClientID in opponent cards entry", it == GlobalState::players.end());
        it->game->cards = entry.second;
    }
}

void GameNode::handleBattleStateUpdate(BattleStateUpdate update) {
    for(auto &player : GlobalState::players) player.game->state = PLAYERSTATE_NONE;

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