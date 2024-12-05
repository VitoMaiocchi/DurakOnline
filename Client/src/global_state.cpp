#include "global_state.hpp"
#include "drawable.hpp"
#include "game_node.hpp"
#include "toplevel_nodes.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>
#include <algorithm>

namespace GlobalState {
    GameState game_state;
    std::set<Player> players;
}

namespace Viewport {
    Extends extends = {0,0,0,0};
    float global_scalefactor = 1000;
    std::unique_ptr<Node> master_node;

    void setup() {
        GlobalState::game_state = GAMESTATE_LOGIN_SCREEN;
        master_node = std::make_unique<LoginScreenNode>(extends);
    }

    void sizeUpdateNotify() {
        extends = {0,0,(float)Viewport::width, (float)Viewport::height};
        global_scalefactor = (width < 1.2*height ? width : height) / 1000.0f; //das chammer no besser mache

        master_node->updateExtends(extends);
    }

    void hoverEventNotify(float x, float y) {
        master_node->sendHoverEvent(x,y);
    }

    void clickEventNotify(float x, float y) {
        master_node->sendClickEvent(x,y);
    }

    void draw() {
        master_node->draw();
    }

    void handleGameStateUpdate(GameStateUpdate update) {
        if(GlobalState::game_state == update.state) return;

        switch(update.state) {
            case GAMESTATE_GAME:
                master_node = std::make_unique<GameNode>(extends);
                break;
            case GAMESTATE_LOBBY:
                master_node = std::make_unique<LobbyNode>(extends);
                break;
            case GAMESTATE_LOGIN_SCREEN:
                //das bruchts nur wenn mer server disconnect handled
                break;
            case GAMESTATE_GAME_OVER:
                master_node = std::make_unique<GameOverScreenNode>();
                master_node->updateExtends(extends); //TODO: das in constructor ine tue
                break;
        }

        GlobalState::game_state = update.state;
    } 

    void handlePlayerUpdate(PlayerUpdate update) {
        for(auto player : GlobalState::players) {
            if(update.player_names.find(player.id) != update.player_names.end()) continue;
            delete player.game;
            GlobalState::players.erase(player);
        }

        for(auto entry : update.player_names) {
            if(GlobalState::players.find({entry.first}) != GlobalState::players.end()) continue;
            const Player p = {
                entry.first,                                    //clientID
                entry.second,                                   //name
                entry.first == update.durak ? true : false,     //durak
                entry.first == GlobalState::clientID ? true : false,         //isYou
                new PlayerGameData()
            };
            GlobalState::players.insert(p);
        }

        if(GlobalState::game_state == GAMESTATE_GAME) cast(GameNode, master_node)->playerUpdateNotify();
        //if(GlobalState::game_state == GAMESTATE_LOBBY) cast(LobbyNode, lobby_node)->playerUpdateNotify();
    }

    void handleMessage(std::unique_ptr<Message> message) {
        switch (message->messageType) {
            case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
                //do nothing
                //print to console for debugs
            break;
            case MESSAGETYPE_CARD_UPDATE:
                throwServerErrorIF("card update can only be processed during game state", GlobalState::game_state != GAMESTATE_GAME);
                cast(GameNode, master_node)->handleCardUpdate(*dynamic_cast<CardUpdate*>(message.get()));
            break;
            case MESSAGETYPE_PLAYER_UPDATE:
                handlePlayerUpdate(*dynamic_cast<PlayerUpdate*>(message.get()));
            break;
            case MESSAGETYPE_BATTLE_STATE_UPDATE:
                cast(GameNode, master_node)->handleBattleStateUpdate(*dynamic_cast<BattleStateUpdate*>(message.get()));
            break;
            case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
                if(GlobalState::game_state == GAMESTATE_GAME)
                    cast(GameNode, master_node)->handleAvailableActionUpdate(*dynamic_cast<AvailableActionUpdate*>(message.get()));
                if(GlobalState::game_state == GAMESTATE_LOBBY) //TODO: das bruchts eig nöd
                    cast(LobbyNode, master_node)->handleAvailableActionUpdate(*dynamic_cast<AvailableActionUpdate*>(message.get()));
            break;
            case MESSAGETYPE_GAME_STATE_UPDATE:
                handleGameStateUpdate(*dynamic_cast<GameStateUpdate*>(message.get()));
            break;
            default:
                //print debug warning: unknown message type
            break;
        }
    }
}