#pragma once 
#include "util.hpp"

#include <string>
#include <utility>
#include <glm/glm.hpp>
#include <functional>

enum TextSize {
    TEXTSIZE_SMALL = 10,
    TEXTSIZE_MEDIUM = 25,
    TEXTSIZE_LARGE = 50,
    TEXTSIZE_XLARGE = 350
};

namespace OpenGL {
    bool setup();
    void drawFrame();
    void cleanup();
    void closeWindow();
    bool windowShouldClose();
    
    void drawImage(std::string path, Extends ext);
    void drawRectangle(Extends ext, glm::vec4 color);
    void drawText(std::string, Extends ext, glm::vec3 color, TextSize size);

    std::pair<uint, uint> getImageDimensions(std::string path);

    void setCharacterInputCallback(std::function<void(char)> callback);
}