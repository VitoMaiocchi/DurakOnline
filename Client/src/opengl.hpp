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

    void drawImage(std::string path, Extends ext);
    void drawRectangle(Extends ext, glm::vec4 color);

    //TODO: ALL DIE CLASSES SIND UNNÖTIG
    //DAS CHAMMER ALLES I NORMALI GLOBALI FUNCTIONS UMWANDLE
    //aka fuck Object Oriented boilerplate cancer
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
            Rectangle(glm::vec3 color);
            Rectangle(glm::vec4 color);
            void draw(Extends ext);
        private:
            glm::vec4 color;
    };

    class Text {
        public:
            Text(std::string text, float r, float g, float b);
            Text(std::string text, glm::vec3 color);
            void draw(float x, float y, float scale_factor);
            void getSize(float &width, float &height);
            std::string text;
            glm::vec3 color;
    };
}