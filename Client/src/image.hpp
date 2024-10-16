#pragma once 

#include <string>
#include <glm/glm.hpp>

void compileImageShaders();
void deleteImageShaders();

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