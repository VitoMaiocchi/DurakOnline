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
#include <fstream>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H


uint Viewport::height = 600;
uint Viewport::width = 800;
uint Window::height = 600;
uint Window::width = 800;

namespace OpenGL {
    MasterNode* masterNode;
    GLFWwindow* window;
    int success;

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
            std::cout << "OpenGL: Shader Compilation Error:\n" << infoLog << std::endl;
        }
    }

    struct Shader {
        uint shader_program;
        uint fragment_shader;
        uint vertex_shader;

        Shader(const char* &vertex_source, const char* &fragment_source) {
            vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            compileShader(vertex_shader, vertex_source);
            compileShader(fragment_shader, fragment_source);

            shader_program = glCreateProgram();
            glAttachShader(shader_program, vertex_shader);
            glAttachShader(shader_program, fragment_shader);
            glLinkProgram(shader_program);
            glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
            if (!success) std::cout << "OpenGL: Shader Linking Error" << std::endl;
        }

        ~Shader() {       
            glDeleteShader(fragment_shader);
            glDeleteShader(vertex_shader);
            glDeleteProgram(shader_program);
        }
    };

    Shader* imageShader;
    Shader* rectangleShader;
    Shader* characterShader;
    unsigned int VAO;

    const float vertices[] = {
        -1.0f,  1.0f, 0.0f,    0.0f, 0.0f,
        1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,    0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 1.0f
    };
    

    //FREE TYPE
    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> Characters;
    unsigned int ftVAO, ftVBO;

    void setupFreeType() {
        characterShader = new Shader(characterVertexShaderSource, characterFragmentShaderSource);


        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Viewport::width), 0.0f, static_cast<float>(Viewport::height));
        glUseProgram(characterShader->shader_program);
        glUniformMatrix4fv(glGetUniformLocation(characterShader->shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // FreeType
        // --------
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return ;
        }

        // find path to font
        std::string font_name = "../fonts/OpenSans-Regular.ttf";
        
        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return;
        }
        else {
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 48);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        
        // configure VAO/VBO for texture quads
        // -----------------------------------
        glGenVertexArrays(1, &ftVAO);
        glGenBuffers(1, &ftVBO);
        glBindVertexArray(ftVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ftVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
        // activate corresponding render state	
        glUseProgram(characterShader->shader_program);
        glUniform3f(glGetUniformLocation(characterShader->shader_program, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(ftVAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) 
        {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },            
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }           
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, ftVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
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

        //Compile Shaders
        imageShader = new Shader(imageVertexShaderSource, imageFragmentShaderSource);
        rectangleShader = new Shader(rectangleVertexShaderSource, rectangleFragmentShaderSource);

        //Create Vertex Array
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
        // texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 

        setupFreeType();

        masterNode = new MasterNode();
        return true;
    }
    
    void startMainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glClearColor(222.0f/255, 93.0f/255, 93.0f/255, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Extends viewport_ext = {0, 0, Viewport::width, Viewport::height, 0};
            masterNode->draw(viewport_ext);

            RenderText("I want mommy I want Milk", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
            RenderText("I have crippeling depression", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

            glfwSwapBuffers(window);
            glfwPollEvents();    
        }
    }

    void closeWindow() {
        glfwSetWindowShouldClose(window, true);
    }

    void cleanup() {
        delete masterNode;
        delete imageShader;
        delete rectangleShader;
        delete characterShader;

        //glDeleteVertexArrays(1, &VAO);
        //glDeleteBuffers(1, &VBO);

        glfwTerminate();
    };


    //IMAGE
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
        glUseProgram(imageShader->shader_program);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3( 2.0f*x/Viewport::width -1.0f, 2.0f*y/Viewport::height -1.0f, layer));
        if(heightb) trans = glm::scale(trans, glm::vec3(size/height*width/Viewport::width, size/Viewport::height, 1.0));
        else trans = glm::scale(trans, glm::vec3(size/Viewport::width, size/width*height/Viewport::height, 1.0));

        unsigned int transformLoc = glGetUniformLocation(imageShader->shader_program, "transform");
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
        glUseProgram(rectangleShader->shader_program);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(
            (2*ext.x - ext.width)/Viewport::width, 
            1 - (2*ext.y + ext.height)/Viewport::height, 
        ext.layer));
        trans = glm::scale(trans, glm::vec3(ext.width/Viewport::width, ext.height/Viewport::height, 1.0));
        unsigned int transformLoc = glGetUniformLocation(rectangleShader->shader_program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        uint colorLoc = glGetUniformLocation(rectangleShader->shader_program, "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}