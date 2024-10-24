#include "master_node.hpp"
#include "viewport.hpp"
#include "shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

uint Viewport::height = 600;
uint Viewport::width = 800;
uint Window::height = 600;
uint Window::width = 800;

namespace OpenGL {
    MasterNode* masterNode;
    GLFWwindow* window;
    int success;

    void compileShaders();
    void deleteShaders();

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xpos = xpos / Window::width * Viewport::width;
            ypos = (1 - ypos / Window::height) * Viewport::height;
            masterNode->sendClickEvent(xpos, ypos);
        }
    }

    void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Viewport::height = height;
        Viewport::width = width;
    }

    void window_size_callback(GLFWwindow* window, int width, int height) {
        Window::height = height;
        Window::width = width;
    }

    
    void compileShader(uint shader, const char* &source) {
        char infoLog[512];
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader Compilation Error:\n" << infoLog << std::endl;
        }
    }
    

    bool setup() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(Window::width, Window::height, "Durak", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        } 

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glViewport(0, 0, Viewport::height, Viewport::width);
        glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
        glfwSetWindowSizeCallback(window, window_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);

        compileShaders();

        masterNode = new MasterNode();
        return true;
    }
    
    void startMainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glClearColor(222.0f/255, 93.0f/255, 93.0f/255, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Extends viewport_ext = {0, 0, Viewport::width, Viewport::height, 0};
            masterNode->draw(viewport_ext);

            glfwSwapBuffers(window);
            glfwPollEvents();    
        }
    }

    void closeWindow() {
        glfwSetWindowShouldClose(window, true);
    }

    void cleanup() {
        delete masterNode;
        glfwTerminate();
    };


    //IMAGE

    const float vertices[] = {
        -1.0f,  1.0f, 0.0f,    0.0f, 0.0f,
        1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,    0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 1.0f
    };

    unsigned int VAO;

    //Image Shaders
    uint imageShaderProgram;
    unsigned int imageVertexShader;
    unsigned int imageFragmentShader;

    uint rectangleShaderProgram;
    unsigned int rectangleVertexShader;
    unsigned int rectangleFragmentShader;
    

    Image::Image(std::string path) {
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
        glUseProgram(imageShaderProgram);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3( 2.0f*x/Viewport::width -1.0f, 2.0f*y/Viewport::height -1.0f, layer));
        if(heightb) trans = glm::scale(trans, glm::vec3(size/height*width/Viewport::width, size/Viewport::height, 1.0));
        else trans = glm::scale(trans, glm::vec3(size/Viewport::width, size/width*height/Viewport::height, 1.0));

        unsigned int transformLoc = glGetUniformLocation(imageShaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    void Image::getDimensions(uint &width, uint &height) {
        width = this->width;
        height = this->height;
    }


    //Rectangle

    Rectangle::Rectangle(float r, float g, float b) {
        color = glm::vec4(r, g, b, 1.0f);
    }

    void Rectangle::draw(Extends ext) {
        glUseProgram(rectangleShaderProgram);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(
            (2*ext.x - ext.width)/Viewport::width, 
            1 - (2*ext.y + ext.height)/Viewport::height, 
        ext.layer));
        trans = glm::scale(trans, glm::vec3(ext.width/Viewport::width, ext.height/Viewport::height, 1.0));
        unsigned int transformLoc = glGetUniformLocation(rectangleShaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        uint colorLoc = glGetUniformLocation(rectangleShaderProgram, "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void compileShaders() {
        char infoLog[512];

        // compile image shaders
        imageVertexShader = glCreateShader(GL_VERTEX_SHADER);
        imageFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        compileShader(imageVertexShader, imageVertexShaderSource);
        compileShader(imageFragmentShader, imageFragmentShaderSource);

        //compile rectangle shaders
        rectangleVertexShader = glCreateShader(GL_VERTEX_SHADER);
        rectangleFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        compileShader(rectangleVertexShader, rectangleVertexShaderSource);
        compileShader(rectangleFragmentShader, rectangleFragmentShaderSource);

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

        //create shader program
        imageShaderProgram = glCreateProgram();
        glAttachShader(imageShaderProgram, imageVertexShader);
        glAttachShader(imageShaderProgram, imageFragmentShader);
        glLinkProgram(imageShaderProgram);
        glGetProgramiv(imageShaderProgram, GL_LINK_STATUS, &success);
        if (!success) std::cout << "SHADER LINKING ERROR" << std::endl;

        rectangleShaderProgram = glCreateProgram();
        glAttachShader(rectangleShaderProgram, rectangleVertexShader);
        glAttachShader(rectangleShaderProgram, rectangleFragmentShader);
        glLinkProgram(rectangleShaderProgram);
        glGetProgramiv(rectangleShaderProgram, GL_LINK_STATUS, &success);
        if (!success) std::cout << "SHADER LINKING ERROR" << std::endl;
    }

    void deleteShaders() {

        /*
        DAS MUSS NO IRGENDWO ANE
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteProgram(shaderProgram);
        */

        glDeleteShader(imageVertexShader);
        glDeleteShader(imageFragmentShader);
    }
}