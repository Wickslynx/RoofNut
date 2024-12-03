#include <stdio.h>
#include <roofnut.h>

//Use OPENGL.
#define ROOFNUT_USE_OPENGL 
//Uncomment to use vulkan.
// #define ROOFNUT_USE_VULKAN

#define ROOFNUT_IMPLEMENTATION

int main() {
    // Application Specification
    ApplicationSpecification appSpec = {
        .name = APPLICATION_NAME,
        .width = 1280,  // Window width
        .height = 720   // Window height
    };

    // Create Application
    Application* app = Application_Create(&appSpec);
    if (!app) {
        fprintf(stderr, "Failed to create application!\n");
        return EXIT_FAILURE;
    }

    main_loop();
    
    Application_Destroy(app);
    cleanup_vulkan();

    return 0;
}
