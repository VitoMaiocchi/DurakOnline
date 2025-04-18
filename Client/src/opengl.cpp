#include "global_state.hpp"

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
#include <exception>
#include <ft2build.h>
#include <chrono>
#include FT_FREETYPE_H

//running the client in the wrong dir is the most common cause of errors.
#define FONT_PATH "../Client/resources/fonts/OpenSans-Bold.ttf"
#define WD_WARN "\n \033[31m    Note: this error is allmost allways caused because you are running Client in the wrong working directory. \n     Plase make sure you are running the Client from build (Client/DurakClient) not build/Client (./DurakClient) \033[0m"

namespace Window {
    unsigned int height = 600;
    unsigned int width = 800;
}

uint Viewport::height = 600;
uint Viewport::width = 800;

namespace OpenGL {
    GLFWwindow* window;

    struct Texture {
        uint gl_texture;
        int width;
        int height;
    };

    struct Shader {
        uint shader_program;
        uint fragment_shader;
        uint vertex_shader;
        Shader(const char* &vertex_source, const char* &fragment_source);
        ~Shader();
    };

    Shader* imageShader;
    Shader* rectangleShader;
    Shader* characterShader;
    unsigned int VAO; //Vertex Array
    int success;

    std::chrono::system_clock::time_point time_stamp;

    bool setupWindow();
    void setupVertexArray();
    void generateCharacterTextures();
    void compileShaders();

    //General Setup

    bool setup() { //Most important function in this file: Main Window Setup
        /*
        Sets up the glfw Window. This includes various callbacks for the following events:
        Window size change
        Viewport size change
        Text input
        Mouse Click
        Mouse Hover
        */
        if(!setupWindow()) return false;

        //compiles the GLSL shaders. This is the code that runs on the graphics card.
        compileShaders();
        /*
        sets up a vertex array of two triangles forming a rectangle
        this is used to render all the primitive objects like 
        Colored Rectangles, Images/Textures and single Characters (text);
        */
        setupVertexArray();
        /*
        This takes a ttf font file from the resource directory and
        creates OpenGl compatible Textures for each character
        (limited to ASCI for simplicity)
        */
        generateCharacterTextures();

        /*
        set initial time stamp and window size
        */
        Viewport::sizeUpdateNotify();
        time_stamp = std::chrono::system_clock::now();
        return true;
    }
    
    void drawFrame() {
        glClearColor(222.0f/255, 93.0f/255, 93.0f/255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto now = std::chrono::system_clock::now();
        uint millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - time_stamp).count();
        if(millis > 50) millis = 50; //in case a frame takes very long, for exmaple establishing network connection
        time_stamp = now;
        Viewport::draw(millis);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    void cleanup() {
        delete imageShader;
        delete rectangleShader;
        delete characterShader;

        glDeleteVertexArrays(1, &VAO);
        //glDeleteBuffers(1, &VBO);

        glfwTerminate();
    };

    void closeWindow() {
        glfwSetWindowShouldClose(window, true);
    }

    bool windowShouldClose() {
        return glfwWindowShouldClose(window);
    }



    //Window Setup

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xpos = xpos / Window::width * Viewport::width;
            ypos = (1 - ypos / Window::height) * Viewport::height;
            Viewport::clickEventNotify(xpos, ypos);
        }
    }

    void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Viewport::height = height;
        Viewport::width = width;
        Viewport::sizeUpdateNotify();
    }

    void window_size_callback(GLFWwindow* window, int width, int height) {
        Window::height = height;
        Window::width = width;
    }

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        xpos = xpos / Window::width * Viewport::width;
        ypos = (1 - ypos / Window::height) * Viewport::height;
        Viewport::hoverEventNotify(xpos, ypos);
    }

    std::function<void(char)> character_input_callback = [](char c){};

    void setCharacterInputCallback(std::function<void(char)> callback) {
        character_input_callback = callback;
    }

    void character_callback(GLFWwindow* window, unsigned int codepoint) {
        if (codepoint <= 0x7F) character_input_callback(static_cast<char>(codepoint));
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) character_input_callback('\b');
        if(key == GLFW_KEY_ENTER && action == GLFW_PRESS) character_input_callback('\n');
    }

    bool setupWindow() {
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
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetCharCallback(window, character_callback);
        glfwSetKeyCallback(window, key_callback);
        return true;
    }

    //Shaders

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
    
    Shader::Shader(const char* &vertex_source, const char* &fragment_source) {
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

    Shader::~Shader() {       
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteProgram(shader_program);
    }


    /*
    Rectangle: The first primitive that can be rendered
    This is very straight forward and only involves one function
    it just sends to color and transform to the graphics card for use in the shader
    and then issues the draw.
    */
    void drawRectangle(Extends ext, glm::vec4 color) {
        glUseProgram(rectangleShader->shader_program);

        glm::mat4 trans = glm::ortho(0.0f, static_cast<float>(Viewport::width), 0.0f, static_cast<float>(Viewport::height));
        trans = glm::translate(trans, glm::vec3(ext.x, ext.y, 0.0));
        trans = glm::scale(trans, glm::vec3(ext.width, ext.height, 1.0));

        unsigned int transformLoc = glGetUniformLocation(rectangleShader->shader_program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        uint colorLoc = glGetUniformLocation(rectangleShader->shader_program, "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    /*
    Image: the second primitive.

    rendred in a simmilar way as the rectangle but this time with a texture.
    To avoid loading the same image file multiple times a pointer to the Texture
    is stored in a hashmap with the path.
    */
    Texture* getTexture(std::string path);

    std::pair<uint, uint> getImageDimensions(std::string path) {
        Texture* texture = getTexture(path);
        return {texture->width, texture->height};
    }

    void drawImage(std::string path, Extends ext) {
        Texture* texture = getTexture(path);

        glBindTexture(GL_TEXTURE_2D, texture->gl_texture);
        glUseProgram(imageShader->shader_program);

        glm::mat4 trans = glm::ortho(0.0f, static_cast<float>(Viewport::width), 0.0f, static_cast<float>(Viewport::height));
        trans = glm::translate(trans, glm::vec3(ext.x, ext.y, 0));
        trans = glm::scale(trans, glm::vec3(ext.width, ext.height, 1.0));

        unsigned int transformLoc = glGetUniformLocation(imageShader->shader_program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

        //Texture creator
    std::map<std::string, Texture> textures;

    Texture* getTexture(std::string path) {
        auto i = textures.find(path);
        if(i != textures.end()) return &i->second;
        
        //load new texture
        Texture tex; 

        glGenTextures(1, &tex.gl_texture);
        glBindTexture(GL_TEXTURE_2D, tex.gl_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int nrChannels;
        unsigned char *data = stbi_load(path.c_str(), &tex.width, &tex.height, &nrChannels, 0);

        if (data){
            if(nrChannels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            if(nrChannels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else throw std::runtime_error("OpenGL: error loading the following texture: "+path+ WD_WARN);
        
        stbi_image_free(data);

        textures[path] = tex;
        return &textures[path];
    }
    

    /*
    Text:

    drawText() draws the text with a given TextSize. If the size is to large
    it fits it within the extends parameter.

    Here the Character textures get pre computed.
    When text is rendered we just iterate trough all the characters and draw them one by one.

    the following parts contain copied code (Refer to the Note in opengl.hpp):
    struct Character();
    generateCharacterTextures();
    renderText(); (modified for use of the same vertex buffer)
    */
    void computeTextSize(std::string text, float scale, float &width, float &height);

    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> Characters;

    void generateCharacterTextures() {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) throw std::runtime_error("FreeType: init error");

        FT_Face face;
        if (FT_New_Face(ft, FONT_PATH, 0, &face)) throw std::runtime_error(std::string("FreeType: failed to load font: ")+FONT_PATH+WD_WARN);

        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 200);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++) {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) throw std::runtime_error("FreeType: failed to load char");

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

        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void computeTextSize(std::string text, float scale, float &width, float &height) {
        width = 0;
        height = 0;
        
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Characters[*c];
            float h = ch.Size.y * scale;
            if(height < h) height = h;
            width += (ch.Advance >> 6) * scale;
        }
    };

    void renderText(std::string text, float x, float y, float scale, glm::vec4 color) {
        glUseProgram(characterShader->shader_program);
        glUniform4f(glGetUniformLocation(characterShader->shader_program, "textColor"), color.x, color.y, color.z, color.w);
        glActiveTexture(GL_TEXTURE0);

        glm::mat4 base_transform = glm::ortho(0.0f, static_cast<float>(Viewport::width), 0.0f, static_cast<float>(Viewport::height));

        // character iterator
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            glm::mat4 transform = glm::translate(base_transform, glm::vec3(xpos, ypos, 0.0f));
            transform = glm::scale(transform, glm::vec3(w, h, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(characterShader->shader_program, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
            
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void drawText(std::string text, Extends ext, glm::vec4 color, TextSize size, TextAlignment align) {
        float scale = Viewport::global_scalefactor * size / 240.0f;

        float width, height;
        computeTextSize(text, scale, width, height);

        if(width > ext.width) {
            scale *= ext.width / width;
            height *= ext.width / width;
            width = ext.width;
        }
        if(height > ext.height) {
            scale *= ext.height / height;
            width *= ext.height / height;
            height = ext.height;
        }

        switch(align) {
            case TEXTALIGN_CENTER:
                renderText(text, ext.x + (ext.width - width)/2, ext.y + (ext.height - height)/2, scale, color);
            break;
            case TEXTALIGN_LEFT:
                renderText(text, ext.x, ext.y + (ext.height - height)/2, scale, color);
        }
    }

    std::pair<float, float> getTextDimensions(std::string text, TextSize size) {
        float scale = Viewport::global_scalefactor * size / 240.0f;
        float width, height;
        computeTextSize(text, scale, width, height);
        return {width, height};
    }

    /*
    This is the Vertex array for a Rectangle made up of two triangles.
    It includes the texture cordinates 
    */
    const float vertices[] = {
        0.0f,  1.0f,   0.0f, 0.0f,
        1.0f,  1.0f,   1.0f, 0.0f,
        1.0f,  0.0f,   1.0f, 1.0f,
        0.0f,  1.0f,   0.0f, 0.0f,
        1.0f,  0.0f,   1.0f, 1.0f,
        0.0f,  0.0f,   0.0f, 1.0f
    };

    void setupVertexArray() {
        //Create Vertex Array
        //Vertex Array 
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }


    /*
    to keep things simple the shader code is just 
    stored in const strings instead of seperate files
    */

    const char *imageVertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 transform;"
        "void main()\n"
        "{\n"
        "   gl_Position = transform * vec4(aPos.x, aPos.y, 1.0, 1.0);\n"
        "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\0";

    const char *imageFragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D texture1;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(texture1, TexCoord);"
        "}\n\0";


    const char *rectangleVertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 transform;"
        "void main()\n"
        "{\n"
        "   gl_Position = transform * vec4(aPos.x, aPos.y, 1.0, 1.0);\n"
        "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\0";

    const char *rectangleFragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   FragColor = color;"
        "}\n\0";

    const char* characterVertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoords;\n"

        "uniform mat4 transform;\n"

        "void main() {"
            "gl_Position = transform * vec4(aPos, 0.0, 1.0);\n"
            "TexCoords = aTexCoord;\n"
        "}";

    const char *characterFragmentShaderSource = 
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"

        "uniform sampler2D text;\n"
        "uniform vec4 textColor;\n"

        "void main() {"
            "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
            "color = textColor * sampled;\n"
        "}";

    void compileShaders() {
        imageShader = new Shader(imageVertexShaderSource, imageFragmentShaderSource);
        rectangleShader = new Shader(rectangleVertexShaderSource, rectangleFragmentShaderSource);
        characterShader = new Shader(characterVertexShaderSource, characterFragmentShaderSource);
    }

}