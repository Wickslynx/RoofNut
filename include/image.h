
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define Image and ImageFormat structs

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels);
GLuint createTexture(const char* filename);