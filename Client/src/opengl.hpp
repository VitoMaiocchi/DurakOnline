#pragma once 
#include "util.hpp"

#include <string>
#include <glm/glm.hpp>

namespace OpenGL {
    bool setup();
    void startMainLoop();
    void cleanup();
    void closeWindow();

    class Image {
        public:
            Image(std::string path);
            void draw(Extends ext);
            void getDimensions(uint &width, uint &height);

        private:
            int shaderProgram;
            unsigned int texture;
            int height, width;
    };

    class Rectangle {
        public:
            Rectangle(float r, float g, float b);
            void draw(Extends ext);
        private:
            glm::vec4 color;
    };
}