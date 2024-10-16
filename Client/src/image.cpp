#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "image.hpp"
#include "viewport.hpp"

float vertices[] = {
    -1.0f,  1.0f, 0.0f,     0.0f, 0.0f,
    1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,     0.0f, 0.0f,
    1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,     0.0f, 1.0f
};

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int VAO;

int success;

Image::Image(std::string path) {
    //create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) std::cout << "SHADER LINKING ERROR" << std::endl;

    //texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //das isch nöd optimal (da sött mer identischi texture recykliere)
    int nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "FAILED TO LOAD TEXTURE: "<< path << std::endl;
    
    stbi_image_free(data);
}

void Image::draw(float x, float y, float size, bool heightb, float layer) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3( 2.0f*x/Viewport::width -1.0f, 2.0f*y/Viewport::height -1.0f, layer));
    if(heightb) trans = glm::scale(trans, glm::vec3(size/height*width/Viewport::width, size/Viewport::height, 1.0));
    else trans = glm::scale(trans, glm::vec3(size/Viewport::width, size/width*height/Viewport::height, 1.0));
    //trans = glm::translate(trans, glm::vec3(x/Viewport::width - 1.0f, y/Viewport::height -1.0f, 0.0f));


    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
};

void Image::getDimensions(uint &width, uint &height) {
    width = this->width;
    height = this->height;
}

/*
DAS MUSS NO IRGENDWO ANE
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
*/

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 transform;"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);"
    "}\n\0";

void compileImageShaders() {
    char infoLog[512];
    // vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "VertexShader Compilation Error:\n" << infoLog << std::endl;
    }

    // fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "FragmentShader Compilation Error:\n" << infoLog << std::endl;
    }

    //Vertex Array 
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
}

void deleteImageShaders() {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}