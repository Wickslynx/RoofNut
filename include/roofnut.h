#ifndef ROOFNUT_H
#define ROOFNUT_H

#ifdef ROOFNUT_IMPLEMENTATION
#include "external/Nuklear/nuklear.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "input.h"
#include "image.h"

#ifdef ROOFNUT_USE_VULKAN
#include "application_vulkan.h"
#include "widgets.h"
#else
#include "application_opengl.h"
#include "widgets.h"
#endif //RoofNut include OpenGL or Vulkan.

#ifdef __cplusplus
}
#endif // C++

#endif // ROOFNUT_IMPLEMENTATION

#endif // ROOFNUT_H
