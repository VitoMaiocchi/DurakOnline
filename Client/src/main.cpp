#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "image.hpp"
#include "viewport.hpp"
#include "drawable.hpp"

uint Viewport::height = 600;
uint Viewport::width = 800;
uint Window::height = 600;
uint Window::width = 800;

//Image* image;

BufferNode node;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        // Get the cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos /= Window::width;
        ypos /= Window::height;
        ypos = 1 - ypos;
        xpos *= Viewport::width;
        ypos *= Viewport::height;
        std::cout << "ViewPort: (" << Viewport::width << ", " << Viewport::height << ")\n";
        std::cout << "Send MouseClickEvent: (" << xpos << ", " << ypos << ")\n";
        node.sendClickEvent(xpos, (double)Viewport::height - ypos);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Durak", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    compileImageShaders();
    //image = new Image("cards/png/ace_of_spades.png");
    //ImageNode imageNode("cards/png/ace_of_spades.png");
    node.child = std::make_shared<ImageNode>("../cards/png/ace_of_spades.png");
    node.child->setClickEventCallback([](){
        std::cout << "KARTE AH CLICKED" << std::endl;
    });
    node.setBufferSize(BUFFERTYPE_ABSOLUTE, 50);

    glViewport(0, 0, Viewport::height, Viewport::width);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Viewport::height = height;
        Viewport::width = width;
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        Window::height = height;
        Window::width = width;
    });


    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        glClearColor(222.0f/255, 93.0f/255, 93.0f/255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //image->draw(0,0, 300, true, 0);
        Extends ext = {0,0, Viewport::width, Viewport::height, 0};
        //imageNode.draw(ext);
        node.draw(ext);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }


    glfwTerminate();  
    return 0;
}