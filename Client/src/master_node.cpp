#include "master_node.hpp"
#include "drawable.hpp"
#include "game_node.hpp"
#include "viewport.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>
#include <algorithm>

bool master_node_exists = false;

GameState GlobalState::game_state = GAMESTATE_NONE;
std::vector<Player> GlobalState::players;

std::unique_ptr<Node> game_node;

MasterNode::MasterNode() {
    assert(!master_node_exists); // Only one master node can exist
    master_node_exists = true;
} 

void MasterNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function) {
    if(GlobalState::game_state == GAMESTATE_GAME) {
        assert(game_node);
        function(game_node);
    }
}

void MasterNode::updateExtends(Extends ext) {
    extends = ext;

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
    Extends ext = {0,0,(float)Viewport::width, (float)Viewport::height};

    if(GlobalState::game_state == GAMESTATE_GAME) {
        game_node = nullptr;
    }

    if(update.state == GAMESTATE_GAME) {
        game_node = std::make_unique<GameNode>(ext);
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

    if(GlobalState::game_state == GAMESTATE_GAME)cast(GameNode, game_node)->playerUpdateNotify();
}

void handleMessage(std::unique_ptr<Message> message) {
    switch (message->messageType) {
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            //do nothing
            //print to console for debugs
        break;
        case MESSAGETYPE_CARD_UPDATE:
            assert(game_node);
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