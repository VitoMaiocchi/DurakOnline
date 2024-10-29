#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "master_node.hpp"

uint clientID = 0;

int main() {
   if(!OpenGL::setup()) return -1;
   clientID = Network::openConnection("localhost", 42069);

   while(!OpenGL::windowShouldClose()) {
      auto m = Network::reciveMessage();
      if(m) handleMessage(std::move(m));
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}