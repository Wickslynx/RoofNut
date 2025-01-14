#include "application_opengl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>





//Additonal Nuklear setup.

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_IMPLEMENTATION
#include "nuklear.h"

#define NK_VULKAN_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"


struct nk_allocator allocator = { 0 }; // Uncomment to use NK, Can't get it to link so don't worry bout it.


#ifdef ROOFNUT_IMPLEMENTATION


 // Note: Uncomment to use Nuklear, I can't get it to link so dont worry bout it for now.
 
 struct nk_context *ctx; 

void init_nuklear(GLFWwindow* window, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkImageView* imageViews, uint32_t imageViewCount, VkFormat format, VkExtent2D extent) { 
	ctx = nk_glfw3_init(window, device, physicalDevice, queueFamilyIndex, imageViews, imageViewCount, format, extent.width, extent.height, queue);
	struct nk_font_atlas *atlas; 
	nk_glfw3_font_stash_begin(&atlas); 
	nk_glfw3_font_stash_end(); 
}

//*/

// Function to initialize OpenGL



extern GLFWwindow *g_Window;

void init_opengl() {
    // Initialize GLFW
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        exit(EXIT_FAILURE);
    }

    // Set GLFW window hints for OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    g_Window = glfwCreateWindow(800, 600, "RoofNut application", NULL, NULL);
    if (!g_Window) {
        printf("Failed to create GLFW window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(g_Window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW.\n");
        exit(EXIT_FAILURE);
    }

    // Set the viewport
    glViewport(0, 0, 800, 600);
}

void RoofNut_loop() {
    init_nuklear();
    init_opengl();

	//Note: OPENGL version does NOT support Nuklear..

	while (!glfwWindowShouldClose(g_Window)) {
        // Poll for and process events
        glfwPollEvents();
		
	
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        nK_glfw3_new_frame();
        UiRender();
        nk_end(ctx);

	
        // Swap front and back buffers
        glfwSwapBuffers(g_Window);
		
    	}
}

void DestroyOpenGl() {
	//No code here rn.
}

Application* Application_Create(const ApplicationSpecification* specification) {
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return NULL;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    Application* app = (Application*)malloc(sizeof(Application));
    if (!app) {
        printf("Allocation of Application failed\n");
        glfwTerminate();
        return NULL;
    }

    app->windowHandle = glfwCreateWindow(specification->width, specification->height, specification->name, NULL, NULL);
    if (!app->windowHandle) {
        printf("Failed to create GLFW window. \n");
        free(app);
        glfwTerminate();
        return NULL;
    }

    g_Window = app->windowHandle;

    return app;
}

void Application_Destroy(Application* app) {
    if (!app) return;
    


	
    DestroyOpenGl();

    glfwDestroyWindow(app->windowHandle);
    glfwTerminate();

}

