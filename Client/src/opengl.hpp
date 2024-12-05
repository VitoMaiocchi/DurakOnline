#pragma once 

/*
        OpenGL
This is where all the actuall graphics are rendered and the Window created.
Each Frame main calls draw frame. DrawFrame() calls Viewport::draw() ViewPort
in GlobalState uses the current gamestate to issue draw calls of Primitives like
(Image, Rectange, Text) to OpenGL. 

Note:
The code here was created with the help of: learnopengl.com
Some minor code snippets may be very simmilar as their example code.

Since Rendering Text in OpenGL is quite tedious most of that code is oughtright copied from here:
https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
Ofcouse with some modifications to get rid of code duplication. For example instead of changing 
the vertex buffer for each character we just use the same vertex buffer for all primitives 
and just update the transform.
*/

#include "util.hpp"

#include <string>
#include <utility>
#include <glm/glm.hpp>
#include <functional>

//different Template Text sizes to make Text Size consisten across components
enum TextSize {
    TEXTSIZE_SMALL = 10,
    TEXTSIZE_MEDIUM = 25,
    TEXTSIZE_LARGE = 50,
    TEXTSIZE_XLARGE = 350
};

namespace OpenGL {
    bool setup(); //sets up the client window
    void drawFrame(); //draws the current frame including everything in the Viewport
    void cleanup();
    void closeWindow();
    bool windowShouldClose();
    
    void drawImage(std::string path, Extends ext);
    void drawRectangle(Extends ext, glm::vec4 color);
    void drawText(std::string, Extends ext, glm::vec3 color, TextSize size);

    std::pair<uint, uint> getImageDimensions(std::string path);

    //used to get user keyboard input
    void setCharacterInputCallback(std::function<void(char)> callback);
}