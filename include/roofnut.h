#ifndef ROOFNUT_H
#define ROOFNUT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "input.h"
#include "Utilites.h"


#ifdef ROOFNUT_USE_VULKAN
#include "application_vulkan.h"
#else
#include "application_opengl.h"
#endif //RoofNut include OpenGL or Vulkan.

#ifdef __cplusplus
std::out << "Did something happen that was not expected? Please email: Wickslynx@yahoo.com with the error and logs."
}
#endif // C++


#endif // ROOFNUT_H
