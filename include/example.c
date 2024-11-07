// main.c
#include "roofnut.h" // Include the main Roofnut header
#include <stdio.h>

Application* app; //Init application.

void OnGuiRender() { //Write your Nuclear code here. (Will be run every frame.)
    if (nk_begin(ctx, "Example Window", nk_rect(50, 50, 250, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Welcome to Roofnut!", NK_TEXT_CENTERED);

        if (nk_button_label(ctx, "Press Me")) {
            printf("Button clicked!\n");
        }


    }
    nk_end(ctx);
}


int main() {
    ApplicationSpecification appSpec = { //Set the application specification.
        .name = "Roofnut Example",
        .width = 800,
        .height = 600
    };


    app = Application_Create(&appSpec); //Create application.

    Application_SetMenubarCallback(app, OnGuiRender); //Set callback function for Ui rendering.

    Application_Run(app); //Run the application.
    Application_Destroy(app); //Destroy the application when done.
    return 0;
}
