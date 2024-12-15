#include "global_state.hpp"
#include "drawable.hpp"
#include "game_node.hpp"
#include "toplevel_nodes.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>
#include <algorithm>
#include <mutex>

#define POPUP_DISTANCE 0.123f
#define POPUP_HEIGHT 0.05f
#define IN_TIME 350
#define OUT_TIME 500
#define POPUP_TEXT_MARGIN 10.0f

namespace GlobalState {
    GameState game_state;
    std::set<Player> players;
}

namespace Viewport {
    Extends extends = {0,0,0,0};
    float global_scalefactor = 1000;
    std::unique_ptr<Node> master_node;

    std::string popup_text = "";
    uint popup_time = 0;
    uint popup_time_end = 0;

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

    void drawPopup(std::string text, uint time, uint end_time) {
        float opacity = 1.0f;
        float fade_in_factor = 1.0f;

        if((time + OUT_TIME) > end_time) opacity = (float) (end_time - time) / OUT_TIME;
        if(time < IN_TIME) fade_in_factor = (float) time / IN_TIME;

        float fade_parabula = 3.79*std::pow(fade_in_factor,3)-8.43*std::pow(fade_in_factor,2)+5.69*fade_in_factor;

        Extends base_ext = {
            extends.x + extends.width*0.25f,
            extends.y + extends.height*(1-POPUP_DISTANCE*fade_parabula),
            extends.width * 0.5f,
            extends.height * POPUP_HEIGHT,
        };

        OpenGL::drawRectangle(base_ext, glm::vec4(0.9f, 0.9f, 0.9f, opacity));
        OpenGL::drawText(text, applyBorder(base_ext, POPUP_TEXT_MARGIN*Viewport::global_scalefactor), glm::vec4(0, 0, 0, opacity), TEXTSIZE_LARGE);
    }

    void createPopup(std::string text, uint seconds) {
        popup_text = text;
        popup_time_end = seconds * 1000;
        popup_time = 0;
    }

    void draw(uint time_delta) {
        master_node->draw();
        
        //popups are drawn seperatly from the main node structure
        if(popup_time_end == 0) return;
        popup_time += time_delta;
        if(popup_time < popup_time_end) {
            drawPopup(popup_text, popup_time, popup_time_end);
            return;
        }
        drawPopup(popup_text, popup_time_end, popup_time_end);
        popup_time = 0;
        popup_time_end = 0;
    }

    //switches the master node depending on game state update sent by the server
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
                //Do nothing (not sent by server)
                break;
            case GAMESTATE_GAME_OVER:
                master_node = std::make_unique<GameOverScreenNode>(extends, 0);
                break;
            case GAMESTATE_SPECTATOR:
                //not implemented initial idea
                break;
            case GAMESTATE_DURAK_SCREEN:
                master_node = std::make_unique<GameOverScreenNode>(extends, 1);
                break;
        }

        GlobalState::game_state = update.state;
    } 

    //updates the player set acording to the update
    void handlePlayerUpdate(PlayerUpdate update) {
        for(auto player : GlobalState::players) {
            if(update.player_names.find(player.id) != update.player_names.end()) continue;
            delete player.game;
            delete player.lobby;
            GlobalState::players.erase(player);
        }

        for(auto entry : update.player_names) {
            if(GlobalState::players.find({entry.first}) != GlobalState::players.end()) continue;
            const Player p = {
                entry.first,                                            //clientID
                entry.second,                                           //name
                entry.first == update.durak ? true : false,             //durak
                entry.first == GlobalState::clientID ? true : false,    //isYou
                new PlayerGameData(),
                new PlayerLobbyData()
            };
            GlobalState::players.insert(p);
        }

        if(GlobalState::game_state == GAMESTATE_GAME) cast(GameNode, master_node)->playerUpdateNotify();
        if(GlobalState::game_state == GAMESTATE_LOBBY) cast(LobbyNode, master_node)->playerUpdateNotify();
    }

    //pass all messages to the responsible function or class
    void handleMessage(std::unique_ptr<Message> message) {
        switch (message->messageType) {
            case MESSAGETYPE_SEND_POPUP:
                createPopup(dynamic_cast<PopupNotify*>(message.get())->message, 3);
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
            break;
            case MESSAGETYPE_GAME_STATE_UPDATE:
                handleGameStateUpdate(*dynamic_cast<GameStateUpdate*>(message.get()));
            break;
            case MESSAGETYPE_READY_UPDATE:
                if(GlobalState::game_state == GAMESTATE_LOBBY)
                    cast(LobbyNode, master_node)->handleReadyUpdate(*dynamic_cast<ReadyUpdate*>(message.get()));
            break;
            case MESSAGETYPE_REMOTE_DISCONNECT_EVENT:
                GlobalState::game_state = GAMESTATE_LOGIN_SCREEN;
                master_node = std::make_unique<LoginScreenNode>(extends);
                createPopup("You got disconnected from the Server", 3);
            break;
            default:
                //unknown message type: do nothing
            break;
        }
    }
}