#pragma once

#include <Networking/message.hpp>
#include "drawable.hpp"
#include <set>

namespace GlobalState {
    //all state information from the server is stored here
    extern GameState game_state;
    extern std::set<Player> players;
    extern ClientID clientID; //client id of this client;
    extern Suit trump_suit;
}

namespace Viewport {
    extern unsigned int height;
    extern unsigned int width;

    extern float global_scalefactor;

    void setup();
    void handleMessage(std::unique_ptr<Message> message);
    void sizeUpdateNotify();
    void hoverEventNotify(float x, float y);
    void clickEventNotify(float x, float y);
    void createPopup(std::string text, uint seconds);
    void draw();
};