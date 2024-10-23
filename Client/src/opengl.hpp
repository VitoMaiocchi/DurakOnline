#pragma once 

#include <string>

namespace OpenGL {
    bool setup();
    void startMainLoop();
    void cleanup();
    void closeWindow();

    class Image {
        public:
            Image(std::string path);
            void draw(float x, float y, float size, bool height, float layer);
            void getDimensions(uint &width, uint &height);

        private:
            int shaderProgram;
            unsigned int texture;
            int height, width;
    };
}