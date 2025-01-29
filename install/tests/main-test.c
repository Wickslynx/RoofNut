
#include <stdio.h>
#include <roofnut.h>

//Debug = On
#define ROOFNUT_DEBUG  

//RoofNut will automaticly detect the right lib.
//Use OpenGL -- If you want to restrict it to a single lib only.
//#define ROOFNUT_USE_OPENGL 
//Uncomment to use vulkan. -- If you want to restrict it to a single lib only.
//#define ROOFNUT_USE_VULKAN
  
#define ROOFNUT_IMPLEMENTAON
//Set this to your applications name.
  
#define APPLICATION_NAME "EXAMPLE"

extern struct nk_context *ctx; 

void RoofNutRender() {
    //Insert your Nuklear code here.
  if (nk_begin(ctx, "", nk_rect(50, 50, 1280, 720),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                     NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
  }
  RenderImage("assets/test-image.png");
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
    struct Application* app = Application_Create(&appSpec);
    if (!app) {
        fprintf(stderr, "Failed to create application!\n");
        return EXIT_FAILURE;
    }
    
    Application_Destroy(app);
    return 0;
}
