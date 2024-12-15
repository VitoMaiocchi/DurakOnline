#pragma once

#include <Networking/message.hpp>
#include "drawable.hpp"
#include <set>

/*
The Global Game State handels all parts that have something to do with Durak Game Logic.
It handles and displays any updates from the server and provides thing like a login and
game screen.

GlobalState and Settings just keep track of some global variables.
All the actual code in global_state.cpp implements Viewport Functions
Viewport essentially represents everything that is visible.
Viewport gets all updates from the OpenGL window or from main.
With the exception of createPopup wich can be called anywhere.

The Viewport is responsible for managing the "master node". 
It represents the entire visible screen. The master node can be any
toplevel node. Toplevel Nodes are nodes that are intended to fill 
the whole screen. They can be found in game_node and toplevel_nodes
*/

//all state information from the server is stored here
namespace GlobalState {
    extern GameState game_state; //Current Game State: game, lobby, etc
    extern std::set<Player> players; //set of players
    extern ClientID clientID; //client id of this client;
    extern Suit trump_suit;
}

namespace Settings {
    extern SortType sortType;
}

namespace Viewport {
    extern unsigned int height;
    extern unsigned int width;

    //dynamically updated scalefactor for things like text and borders 
    //this is updated whenever the window size changes
    extern float global_scalefactor;

    void setup(); //set the default game state to login screen (called once by main)
    void handleMessage(std::unique_ptr<Message> message); //handles incoming messages from the server (called from main)
    void sizeUpdateNotify(); //called by OpenGl when the viewport size changes. Updates scalefactor and master node extends
    void hoverEventNotify(float x, float y); //sends hoverevent to masternode
    void clickEventNotify(float x, float y); //sends click event to masternode
    void createPopup(std::string text, uint seconds); //displays a popup 
    void draw(uint time_delta); //draws the current frame (frame time as argument)
}