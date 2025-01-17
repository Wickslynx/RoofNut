#include "application_opengl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include "external/glfw/include/GLFW/glfw3.h"



extern GLFWwindow *g_Window;
GLFWwindow* g_Window = NULL;


struct Application {
    struct ApplicationSpecification specification;
    bool running;
    bool customTitleBar;
    GLFWwindow* windowHandle;
};

//Additonal Nuklear setup.

#ifdef ROOFNUT_NUKLEAR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_IMPLEMENTATION
#include "external/Nuklear/nuklear.h"

#define NK_VULKAN_IMPLEMENTATION
#include "external/Nuklear/nuklear_glfw_gl3.h"


struct nk_allocator allocator = { 0 }; // Uncomment to use NK, Can't get it to link so don't worry bout it.





 // Note: Uncomment to use Nuklear, I can't get it to link so dont worry bout it for now.
 
struct nk_context *ctx; 
struct nk_font_atlas *atlas;
struct nk_glfw glfw;

void init_nuklear(GLFWwindow* window) {
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

    /* Load Fonts */
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    /* If you have a ttf file, you can add it here */
    /* E.g., struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "path/to/your/font.ttf", 14, 0); */
    nk_glfw3_font_stash_end(&glfw);
}


//*/

// Function to initialize OpenGL


#endif

extern GLFWwindow *g_Window;
void init_opengl() {
    // Initialize GLFW
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        exit(EXIT_FAILURE);
    }

    // Set GLFW window hints before window creation
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

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
    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glewInitResult));
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Now that GLEW is initialized, we can safely check OpenGL version
    const GLubyte* glVersion = glGetString(GL_VERSION);
    if (glVersion == NULL) {
        printf("Failed to retrieve OpenGL version.\n");
    } else {
        printf("OpenGL version: %s\n", glVersion);
    }

    // Set the viewport
    glViewport(0, 0, 800, 600);

    // Enable debug output if available
    if (GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
}

void RoofNut_loop() {
    #ifdef ROOFNUT_NUKLEAR
    init_nuklear(g_Window);
    #endif

	//Note: OPENGL version does NOT support Nuklear..

	while (!glfwWindowShouldClose(g_Window)) {
        // Poll for and process events
        glfwPollEvents();
		
	
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);
		
        #ifdef ROOFNUT_NUKLEAR
        nk_glfw3_new_frame(&glfw);
        UiRender();
        nk_end(ctx);
        #endif

	
        // Swap front and back buffers
        glfwSwapBuffers(g_Window);
		
    	}
}

void DestroyOpenGl() {
	//No code here rn.
}


struct Application* Application_Create(const struct ApplicationSpecification* specification) {
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return NULL;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    struct Application* app = (struct Application*)malloc(sizeof(struct Application));
    if (!app) {
        printf("Allocation of Application failed\n");
        glfwTerminate();
        return NULL;
    }

    app->windowHandle = glfwCreateWindow(specification->width, specification->height, specification->name, NULL, NULL);
    if (!app->windowHandle) {
        printf("Failed to create GLFW window.\n");
        free(app);
        glfwTerminate();
        return NULL;
    }

    g_Window = app->windowHandle;
    init_opengl();

    return app;
}


void Application_Destroy(struct Application* app) {
    if (!app) return;
    


	
    DestroyOpenGl();

    glfwDestroyWindow(app->windowHandle);
    glfwTerminate();

}
