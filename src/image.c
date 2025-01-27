#include <stdlib.h>
#include <stdio.h>
#include "image.h"



unsigned char* loadImage(const char* filename, int* width, int* height, int* channels) {
    return stbi_load(filename, width, height, channels, 0);
}

GLuint createTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = loadImage(filename, &width, &height, &channels);
    if (!data) {
        printf("Failed to load image: %s\n", filename);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

void RenderImage(const char* imagePath) {
    GLuint texture = createTexture(imagePath);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);

    
}