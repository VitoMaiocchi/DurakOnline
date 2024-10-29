#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "master_node.hpp"

int main() {
   if(!OpenGL::setup()) return -1;

   while(!OpenGL::windowShouldClose()) {
      //poll messages
      auto m = Network::reciveMessage();
      if(m) handleMessage(std::move(m));
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}