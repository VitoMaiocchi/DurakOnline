#include "master_node.hpp"
#include "drawable.hpp"
#include "game_node.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>
#include <algorithm>

bool master_node_exists = false;

GameState GlobalState::game_state = GAMESTATE_NONE;
std::vector<Player> GlobalState::players;

//std::unique_ptr<Node> node;
//std::unique_ptr<Node> node2;
std::unique_ptr<Node> rect_node;
std::unique_ptr<Node> rect_node2;
std::unique_ptr<Node> text_node;
std::unique_ptr<Node> stack_node1;

std::unique_ptr<Node> game_node;

MasterNode::MasterNode() {
    assert(!master_node_exists); // Only one master node can exist
    master_node_exists = true;
    /*
    // BufferNode for Ace of Spades
    node = std::make_unique<BufferNode>();
    auto bufferNode = cast(BufferNode, node);
    if (bufferNode) {
        bufferNode->child = std::make_unique<ImageNode>(Card(RANK_ACE, SUIT_SPADES).getFileName());
        bufferNode->setBufferSize(BUFFERTYPE_RELATIVE, 50);
        bufferNode->child->setClickEventCallback([]() {
            std::cout << "Clicked on Ace of Spades" << std::endl;
        });
    } else {
        std::cerr << "Failed to cast node to BufferNode" << std::endl;
    }
//
    // BufferNode for Queen of Diamonds
    node2 = std::make_unique<BufferNode>();
    auto bufferNode2 = cast(BufferNode, node2);
    if (bufferNode2) {
        bufferNode2->child = std::make_unique<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName());
        bufferNode2->setBufferSize(BUFFERTYPE_ABSOLUTE, 50);
        bufferNode2->child->setClickEventCallback([]() {
            std::cout << "Clicked on Queen of Diamonds" << std::endl;
        });
    } else {
        std::cerr << "Failed to cast node2 to BufferNode" << std::endl;
    }
    */
    // RectangleNode and TextNode
    rect_node = std::make_unique<RectangleNode>(0, 0, 0);
    rect_node2 = std::make_unique<RectangleNode>(1.0, 0, 0);
    text_node = std::make_unique<TextNode>("Sample Text", 0.2, 1.0, 0.8);

    // StackNode
    stack_node1 = std::make_unique<LinearStackNode>();
    cast(LinearStackNode, stack_node1)->children.push_back(std::make_unique<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName()));
    cast(LinearStackNode, stack_node1)->children.push_back(std::make_unique<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName()));
    cast(LinearStackNode, stack_node1)->children.push_back(std::make_unique<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName()));
    std::cerr << "MasterNode initialization complete." << std::endl;
} 

void MasterNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    function(rect_node);
    function(rect_node2);
    function(text_node);
    function(stack_node1);
    if(GlobalState::game_state == GAMESTATE_GAME) {
        assert(game_node);
        function(game_node);
    }
}

void MasterNode::updateExtends(Extends ext) {
    if (rect_node) {
        rect_node->updateExtends(ext);
    } else {
        std::cerr << "Warning: rect_node is null in updateExtends." << std::endl;
    }

    if (rect_node2) {
        rect_node2->updateExtends(ext);
    } else {
        std::cerr << "Warning: rect_node2 is null in updateExtends." << std::endl;
    }

    if (text_node) {
        text_node->updateExtends(ext);
    } else {
        std::cerr << "Warning: text_node is null in updateExtends." << std::endl;
    }
    extends = ext;
    Extends ext1 = {ext.x, ext.y, ext.width/2, ext.height};
    Extends ext2 = {ext.x+ext.width/2, ext.y, ext.width/2, ext.height};
    rect_node->updateExtends(ext1);
    rect_node2->updateExtends(ext2);
    text_node->updateExtends(ext);
    stack_node1->updateExtends(ext);

    if(GlobalState::game_state == GAMESTATE_GAME) {
        assert(game_node);
        game_node->updateExtends(ext);
    }
}

Extends MasterNode::getCompactExtends(Extends ext) {
    return ext;
}

void handleGameStateUpdate(GameStateUpdate update) {
    if(GlobalState::game_state == update.state) return;

    if(GlobalState::game_state == GAMESTATE_GAME) {
        game_node = std::make_unique<GameNode>();
    }

    if(update.state == GAMESTATE_GAME) {
        game_node = nullptr;
    }

    GlobalState::game_state = update.state;
    //TODO ...
} 

void handlePlayerUpdate(PlayerUpdate update) {
    GlobalState::players = {};
    for(auto entry : update.player_names) {
        Player p = {
            entry.first,                                    //clientID
            entry.second,                                   //name
            entry.first == update.durak ? true : false,     //durak
            entry.first == clientID ? true : false,         //isYou
        };
        GlobalState::players.push_back(p);
    }

    //sort by player_id with you at the beginning
    std::sort(GlobalState::players.begin(), GlobalState::players.end(), [](Player a, Player b) {
        if(a.is_you) return true;
        if(b.is_you) return false;
        return a.id > b.id;
    });

    cast(GameNode, game_node)->playerUpdateNotify();
}

void handleMessage(std::unique_ptr<Message> message) {
    switch (message->messageType) {
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            //do nothing
            //print to console for debugs
        break;
        case MESSAGETYPE_CARD_UPDATE:
            cast(GameNode, game_node)->handleCardUpdate(*dynamic_cast<CardUpdate*>(message.get()));
        break;
        case MESSAGETYPE_PLAYER_UPDATE:
            handlePlayerUpdate(*dynamic_cast<PlayerUpdate*>(message.get()));
        break;
        case MESSAGETYPE_BATTLE_STATE_UPDATE:
            cast(GameNode, game_node)->handleBattleStateUpdate(*dynamic_cast<BattleStateUpdate*>(message.get()));
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            if(GlobalState::game_state == GAMESTATE_GAME && GlobalState::game_state == GAMESTATE_LOBBY)
                cast(GameNode, game_node)->handleAvailableActionUpdate(*dynamic_cast<AvailableActionUpdate*>(message.get()));
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            handleGameStateUpdate(*dynamic_cast<GameStateUpdate*>(message.get()));
        break;
        default:
            //print debug warning: unknown message type
        break;
    }
}