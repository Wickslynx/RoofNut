#include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define Image and ImageFormat structs
typedef enum {
    IMAGE_FORMAT_RGBA,
    IMAGE_FORMAT_RGBA32F
} ImageFormat;

typedef struct {
    char* filepath;
    int width;
    int height;
    ImageFormat format;
    GLuint textureID;
} Image;


// Utility functions
static uint32_t Utils_BytesPerPixel(ImageFormat format);
static GLuint Utils_LoadTexture(const char* filepath, int* width, int* height, ImageFormat* format);
Image* Image_Create(const char* path);
void Image_Destroy(Image* img);
void Image_Render(Image* img, float x, float y, float width, float height);

static uint32_t Utils_BytesPerPixel(ImageFormat format) {
    switch (format) {
        case IMAGE_FORMAT_RGBA:    return 4;
        case IMAGE_FORMAT_RGBA32F: return 16;
        default:                   return 0;
    }
}
