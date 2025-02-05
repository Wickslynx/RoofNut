#ifndef IMAGE_H
#define IMAGE_H

#ifdef ROOFNUT_USE_OPENGL
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
ImageRenderer* createImageRenderer(const char* filename, int width, int height, float posX, float posY);
void RenderImage(const char* filename, int width, int height, float posX, float posY);
void renderImage(ImageRenderer* renderer);
void destroyImageRenderer(ImageRenderer* renderer);

#else

void RenderImage(const char* filename, int width, int height, float posX, float posY);

#endif //ROOFNUT_USE_OPENGl

#endif //IMAGE_H
