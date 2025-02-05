#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROOFNUT_IMPLEMENTATION //If not defined, roofnut wont be able to be used.
#define ROOFNUT_DEBUG //Set roofnut to debug mode.
//ROOFNUT_IMAGE_SUPPORT_OFF //Remove the image support, (slight speed improvment.)
//ROOFNUT_USE_VULKAN // (Restrict it to only Vulkan)
//ROOFNUT_USE_OPENGL // (Restrict it to only OpenGL)

#include <roofnut.h>

  
//Set this to your applications name.
#define APPLICATION_NAME "EXAMPLE"

extern struct nk_context *ctx; 

void RoofNutRender() {
  //Insert your rendering code here. (Nuklear and image rendering).

  if (nk_begin(ctx, "", nk_rect(50, 50, 1280, 720),
                     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
  }

  RenderImage("path-to-image", 100, 100, 100, 100);
 
  nk_end(ctx);

  
}

int main() {
    // Application Specification
    struct ApplicationSpecification appSpec = {
        .name = APPLICATION_NAME,
        .width = 1280,  // Window width
        .height = 720   // Window height
    };

    // Create Application
    struct Application* app = RoofNutCreate(&appSpec);
    if (!app) {
        fprintf(stderr, "Failed to create application!\n");
        return EXIT_FAILURE;
    }
    
    RoofNutDestroy(app);
    return 0;
}
