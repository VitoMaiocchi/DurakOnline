#include "opengl.hpp"

int main() {
   if(!OpenGL::setup()) return -1;

   OpenGL::startMainLoop();
   OpenGL::cleanup();

   return 0;
}