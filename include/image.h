#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typdef struct Image {
  void (*SetData)(struct Image *self)
  void (*resize)(struct Image *self)
  void (*GetWidth)(struct Image *self)
  void (*GetHeight)(struct Image *self)
  void (*AllocateMemory)(struct Image *self)
  void (*Release)(struct Image *self)
} Image;

typedef enum {
    int None = 0;
    int RGBA, RGBA32F;
  } ImageFormat;
