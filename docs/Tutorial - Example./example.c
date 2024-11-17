
#include <roofnut.h>
#include <stdio.h>

Application* app; //Init application.

void OnUiRender() { //Write your Nuclear code here. (Will be run every frame.)
    
}


int main() {
    ApplicationSpecification appSpec = { //Set the application specification.
        .name = "Roofnut Example",
        .width = 800,
        .height = 600
    };


    app = Application_Create(&appSpec); //Create application.

    Application_SetMenubarCallback(app, OnUiRender); //Set callback function for Ui rendering.

    Application_Run(app); //Run the application.
    Application_Destroy(app); //Destroy the application when done.
    return 0;
}
