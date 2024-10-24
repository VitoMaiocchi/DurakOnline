#pragma once
/*
to keep things simple the shader code is 
just stored in const strings
*/

const char *imageVertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 transform;"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *imageFragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);"
    "}\n\0";


const char *rectangleVertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 transform;"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *rectangleFragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = color;"
    "}\n\0";

const char* characterVertexShaderSource = 
    "#version 420\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"

    "uniform mat4 projection;\n"

    "void main() {"
        "gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "TexCoords = vertex.zw;\n"
    "}";

const char *characterFragmentShaderSource = 
    "#version 420\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"

    "uniform sampler2D text;\n"
    "uniform vec3 textColor;\n"

    "void main() {"
        "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "color = vec4(textColor, 1.0) * sampled;\n"
    "}";
