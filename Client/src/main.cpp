#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "global_state.hpp"

ClientID GlobalState::clientID = 0;

/*
DURAK CLIENT
here is a rough overview over the high level functions in the DurakClient

The DurakClient Code is made up of 3 main components.

1. The Node Framework
   (drawable.cpp)

2. The Global Game State including any child Nodes
   (gloabl_state.cpp, game_node.cpp toplevel_nodes.cpp) 

3. OpenGl this includes the Window and any actuall rendering
   (opengl.cpp)

The Node Framework Provides a way to stucture anything displayed on screen
in a hirachy of nodes. Any updates in the Window size propagte trough the node tree.
The size of each Node in the tree is only calulated when the window size changes.
This Extends of the Node is used to handle drawing and mouse events like hover/click.
This makes everything scale dynamically
(more detail in drawable.hpp)

The Global Game State handels all parts that have something to do with Durak Game Logic.
It handles and displays any updates from the server and provides thing like a login and
game screen.
(more detail in global_state.hpp)

OpenGL is the graphics library used to render everything efficiently. To create a Window
glfw is used. opengl.cpp provied a way to render prmitives like Text, Images and Rectangles
to the screen each frame
(more detail in opengl.hpp)


here in main is the main loop:
*/

int main() {
   //setup any game logic components (Viewport)
   Viewport::setup();
   //setup the window and open rendering
   if(!OpenGL::setup()) return -1;

   //start game loop
   while(!OpenGL::windowShouldClose()) {
      //recive new messages from server
      auto m = Network::reciveMessage();
      //if a new message has been recived Pass it to the game logic (Viewport in global_state)
      //m is null if no new messages have been recived or 
      //if there is currently no connection to the server
      if(m) Viewport::handleMessage(std::move(m));
      //draw the frame
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}