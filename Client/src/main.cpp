#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "master_node.hpp"

ClientID clientID = 0;

int main() {
   if(!OpenGL::setup()) return -1;

   while(!OpenGL::windowShouldClose()) {
      auto m = Network::reciveMessage();
      if(m) {
         if (m->messageType == MESSAGETYPE_REMOTE_DISCONNECT_EVENT) break;
         handleMessage(std::move(m));
      }
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}