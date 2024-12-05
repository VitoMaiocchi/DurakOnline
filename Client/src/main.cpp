#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "global_state.hpp"

ClientID GlobalState::clientID = 0;

int main() {
   Viewport::setup();
   if(!OpenGL::setup()) return -1;

   while(!OpenGL::windowShouldClose()) {
      auto m = Network::reciveMessage();
      if(m) {
         if (m->messageType == MESSAGETYPE_REMOTE_DISCONNECT_EVENT) break;
         Viewport::handleMessage(std::move(m));
      }
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}