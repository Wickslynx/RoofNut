#ifndef ROOFNUT_IMAGE_SUPPORT_OFF

#ifdef ROOFNUT_USE_VULKAN
#include <vulkan/vulkan.h>
#else 
#include <GL/glew.h>
#endif


#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef ROOFNUT_USE_OPENGL
ImageRenderer* imageRenderer = NULL;  

// Vertex shader
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\0";

// Fragment shader
const char* fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(texture1, TexCoord);\n"
    "}\0";

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels) {
    stbi_set_flip_vertically_on_load(true);  // Flip image vertically for OpenGL
    unsigned char* data = stbi_load(filename, width, height, channels, 0);
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
    return data;
}

GLuint createShaderProgram() {
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "Vertex shader compilation failed: %s\n", infoLog);
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "Fragment shader compilation failed: %s\n", infoLog);
    }

    // Shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check program linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

ImageRenderer* createImageRenderer(const char* filename, int width, int height, float posX, float posY) {
    ImageRenderer* renderer = (ImageRenderer*)malloc(sizeof(ImageRenderer));
    if (!renderer) return NULL;

    int channels;
    unsigned char* data = loadImage(filename, &width, &height, &channels);
    if (!data) {
        free(renderer);
        return NULL;
    }

    // Create texture
    glGenTextures(1, &renderer->textureID);
    glBindTexture(GL_TEXTURE_2D, renderer->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0,
                 channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // Create shader program
    renderer->shaderProgram = createShaderProgram();

    // Vertex data with dynamic position
    float vertices[] = {
        // positions                  // texture coords
        posX - 0.5f, posY - 0.5f, 0.0f,  0.0f, 0.0f,  // bottom left
        posX + 0.5f, posY - 0.5f, 0.0f,  1.0f, 0.0f,  // bottom right
        posX + 0.5f, posY + 0.5f, 0.0f,  1.0f, 1.0f,  // top right
        posX - 0.5f, posY + 0.5f, 0.0f,  0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    // Create buffers
    glGenVertexArrays(1, &renderer->VAO);
    glGenBuffers(1, &renderer->VBO);
    glGenBuffers(1, &renderer->EBO);

    glBindVertexArray(renderer->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return renderer;
}

void renderImage(ImageRenderer* renderer) {
    if (!renderer) return;

    glUseProgram(renderer->shaderProgram);
    glBindTexture(GL_TEXTURE_2D, renderer->textureID);
    glBindVertexArray(renderer->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderImage(const char* filename, int width, int height, float posX, float posY) {
    imageRenderer = createImageRenderer(filename, width, height, posX, posY);
}

void destroyImageRenderer(ImageRenderer* renderer) {
    if (!renderer) return;

    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteBuffers(1, &renderer->EBO);
    glDeleteTextures(1, &renderer->textureID);
    glDeleteProgram(renderer->shaderProgram);
    free(renderer);
}

#else

void RenderImage(const char* filename, int width, int height, float posX, float posY) {
    printf("(WARNING) Images is not supported in the vulkan version, will not be rendered.");
}

#endif //ROOFNUT_USE_OPENGL

#endif //ROOFNUT_IMAGE_SUPPORT_OFF
