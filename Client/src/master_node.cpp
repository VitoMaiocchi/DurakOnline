#include "master_node.hpp"
#include "drawable.hpp"
#include "game_node.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>
#include <list>
#include <algorithm>

bool master_node_exists = false;

std::shared_ptr<BufferNode> node;
std::shared_ptr<BufferNode> node2;
std::shared_ptr<RectangleNode> rect_node;
std::shared_ptr<RectangleNode> rect_node2;

std::shared_ptr<GameNode> game_node;

MasterNode::MasterNode() {
    assert(!master_node_exists); //only one master node can exist
    master_node_exists = true;

    node = std::make_shared<BufferNode>();
    node->child = std::make_shared<ImageNode>(Card(RANK_ACE, SUIT_SPADES).getFileName());
    node->setBufferSize(BUFFERTYPE_RELATIVE, 50);
    node->child->setClickEventCallback([]() {
        std::cout << "Clicked on Ace of Spades" << std::endl;
    });

    node2 = std::make_shared<BufferNode>();
    node2->child = std::make_shared<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName());
    node2->setBufferSize(BUFFERTYPE_ABSOLUTE, 50);
    node2->child->setClickEventCallback([]() {
        std::cout << "Clicked on Queen of Diamonds" << std::endl;
    });

    rect_node = std::make_shared<RectangleNode>(0, 0, 0);
    rect_node2 = std::make_shared<RectangleNode>(1.0, 0, 0);
}

void MasterNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    function(rect_node);
    function(rect_node2);
    function(node);
    function(node2);

    if(GlobalState::game_state == GAMESTATE_GAME) {
        assert(game_node);
        function(game_node);
    }
}

void MasterNode::updateExtends(Extends ext) {
    extends = ext;
    Extends ext1 = {ext.x, ext.y, ext.width/2, ext.height};
    Extends ext2 = {ext.x+ext.width/2, ext.y, ext.width/2, ext.height};
    rect_node->updateExtends(ext1);
    rect_node2->updateExtends(ext2);
    node->updateExtends(ext1);
    node2->updateExtends(ext2);

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
        game_node = std::make_shared<GameNode>();
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

    game_node->playerUpdateNotify();
}

void handleMessage(std::unique_ptr<Message> message) {
    switch (message->messageType) {
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            //do nothing
            //print to console for debugs
        break;
        case MESSAGETYPE_CARD_UPDATE:
            game_node->handleCardUpdate(*dynamic_cast<CardUpdate*>(message.get()));
        break;
        case MESSAGETYPE_PLAYER_UPDATE:
            handlePlayerUpdate(*dynamic_cast<PlayerUpdate*>(message.get()));
        break;
        case MESSAGETYPE_BATTLE_STATE_UPDATE:
            game_node->handleBattleStateUpdate(*dynamic_cast<BattleStateUpdate*>(message.get()));
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            if(GlobalState::game_state == GAMESTATE_GAME && GlobalState::game_state == GAMESTATE_LOBBY)
                game_node->handleAvailableActionUpdate(*dynamic_cast<AvailableActionUpdate*>(message.get()));
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            handleGameStateUpdate(*dynamic_cast<GameStateUpdate*>(message.get()));
        break;
        default:
            //print debug warning: unknown message type
        break;
    }
}