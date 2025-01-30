#ifndef IMAGE_H
#define IMAGE_H

#include <GL/gl.h>
#include <stdbool.h>


typedef struct {
    GLuint textureID;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint shaderProgram;
} ImageRenderer;

extern ImageRenderer* imageRenderer;

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels);
ImageRenderer* createImageRenderer(const char* filename);
void RenderImage(const char* filename);
void renderImage(ImageRenderer* renderer);
void destroyImageRenderer(ImageRenderer* renderer);

#endif
