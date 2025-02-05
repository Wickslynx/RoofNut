
#include <stdio.h>
#include <stdlib.h>

#define ROOFNUT_TEST_FILE
#define ROOFNUT_IMPLEMENTATION
#include <roofnut.h>

  
#define APPLICATION_NAME "EXAMPLE"

extern struct nk_context *ctx; 

void RoofNutRender() {

  if (nk_begin(ctx, "", nk_rect(50, 50, 1280, 720),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                     NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
  }
  RenderImage("assets/test-image.png", 100, 200, 0.0, 0.0);
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
        return 0;
    }
    
    RoofNutDestroy(app);
    return 0;
}
