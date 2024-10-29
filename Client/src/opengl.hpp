#pragma once 
#include "util.hpp"

#include <string>
#include <glm/glm.hpp>

namespace OpenGL {
    bool setup();
    void drawFrame();
    void cleanup();
    void closeWindow();
    bool windowShouldClose();

    struct Texture;

    class Image {
        public:
            Image(std::string path);
            void draw(Extends ext);
            void getDimensions(uint &width, uint &height);

        private:
            Texture* texture;
    };

    class Rectangle {
        public:
            Rectangle(float r, float g, float b);
            void draw(Extends ext);
        private:
            glm::vec4 color;
    };
}