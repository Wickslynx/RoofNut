#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "stb_image.h"


static GLuint Utils_LoadTexture(const char* filepath, int* width, int* height, ImageFormat* format) {
    int channels;
    uint8_t* data = stbi_load(filepath, width, height, &channels, 4);
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filepath);
        return 0;
    }

    *format = IMAGE_FORMAT_RGBA;
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

Image* Image_Create(const char* path) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->filepath = strdup(path);

    img->textureID = Utils_LoadTexture(img->filepath, &img->width, &img->height, &img->format);
    if (img->textureID == 0) {
        free(img->filepath);
        free(img);
        return NULL;
    }

    return img;
}

void Image_Destroy(Image* img) {
    if (!img) return;
    glDeleteTextures(1, &img->textureID);
    free(img->filepath);
    free(img);
}

void Image_Render(Image* img, float x, float y, float width, float height) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, img->textureID);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
