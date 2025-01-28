
#include <GL/gl.h>

// Define Image and ImageFormat structs

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels);
GLuint createTexture(const char* filename);
void RenderImage(const char* imagePath);
