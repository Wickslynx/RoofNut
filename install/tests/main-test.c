
#include <stdio.h>

#define ROOFNUT_IMPLEMENTAON
#include <roofnut.h>

  
#define APPLICATION_NAME "EXAMPLE"

extern struct nk_context *ctx; 

void RoofNutRender() {

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
