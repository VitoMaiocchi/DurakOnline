#define NETWORKTYPE_CLIENT
#include <Networking/network.hpp>
#include "opengl.hpp"
#include "master_node.hpp"

ClientID clientID = 0;

int main() {
   if(!OpenGL::setup()) return -1;
   //clientID = Network::openConnection("localhost", 42069);

   //für debug vo GameNode
   GameStateUpdate update;
   update.state = GAMESTATE_GAME;
   MessagePtr message = std::make_unique<GameStateUpdate>(update);
   handleMessage(std::move(message));

   CardUpdate update2;
   update2.hand = {
      Card(RANK_ACE, SUIT_SPADES),
      Card(RANK_KING, SUIT_DIAMONDS),
      Card(RANK_QUEEN, SUIT_HEARTS),
      Card(RANK_JACK, SUIT_CLUBS)
   };
   message = std::make_unique<CardUpdate>(update2);
   handleMessage(std::move(message));
   //end debug

   while(!OpenGL::windowShouldClose()) {
      //auto m = Network::reciveMessage();
      //if(m) handleMessage(std::move(m));
      OpenGL::drawFrame();
   }
   OpenGL::cleanup();

   return 0;
}