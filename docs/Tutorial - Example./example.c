#include <stdio.h>
#include <roofnut.h>

//Debug = On
#define ROOFNUT_DEBUG 

//Use OpenGL
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

    RoofNut_loop(); //Will render a Window.
    
    Application_Destroy(app);

    return 0;
}
