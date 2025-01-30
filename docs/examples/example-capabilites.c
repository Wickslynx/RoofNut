
#include <stdio.h>

#define ROOFNUT_IMPLEMENTATION
#include <roofnut.h>


extern nk_context* ctx;

static struct {
    struct nk_colorf color;  // Changed to nk_colorf
    int slider_value;
    char text[256];
    int selected_item;
    bool show_about;
    struct nk_colorf background_color;
} ui_state = {
    .color = {1.0f, 0.0f, 0.0f, 1.0f},
    .slider_value = 50,
    .text = "",
    .selected_item = 0,
    .show_about = false,
    .background_color = {0.10f, 0.18f, 0.24f, 1.0f}
};


#define APPLICATION_NAME "RoofNut-Example-Application"

void RoofNutRender() {
    if (!ctx) return;

    // Main Menu Bar
    if (nk_begin(ctx, "Menu Bar", nk_rect(0, 0, 1920, 30), 
        NK_WINDOW_NO_SCROLLBAR)) {
        nk_menubar_begin(ctx);
        
        // File Menu
        if (nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "New", NK_TEXT_LEFT))
                printf("New project\n");
            if (nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT))
                printf("Open project\n");
            if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT))
                printf("Save project\n");
            nk_menu_end(ctx);
        }
        
        // Edit Menu
        if (nk_menu_begin_label(ctx, "Edit", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Copy", NK_TEXT_LEFT))
                printf("Copy\n");
            if (nk_menu_item_label(ctx, "Paste", NK_TEXT_LEFT))
                printf("Paste\n");
            nk_menu_end(ctx);
        }
        
        // Help Menu
        if (nk_menu_begin_label(ctx, "Help", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "About", NK_TEXT_LEFT))
                ui_state.show_about = true;
            nk_menu_end(ctx);
        }
        
        nk_menubar_end(ctx);
    }
    nk_end(ctx);

    // Tools Panel (Left Side)
    if (nk_begin(ctx, "Tools", nk_rect(0, 30, 250, 670),
        NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {
        
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Color Picker:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 180, 1);
        ui_state.color = nk_color_picker(ctx, ui_state.color, NK_RGBA);  // Fixed color picker
        
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Slider:", NK_TEXT_LEFT);
        nk_slider_int(ctx, 0, &ui_state.slider_value, 100, 1);
        
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Input:", NK_TEXT_LEFT);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ui_state.text, 
            sizeof(ui_state.text) - 1, nk_filter_default);  // Leave space for null terminator
    }
    nk_end(ctx);

    // Properties Panel (Right Side)
    if (nk_begin(ctx, "Properties", nk_rect(1670, 30, 250, 670),
        NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {
        
        const char *items[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Select Item:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        
        for (int i = 0; i < 4; ++i) {
            if (nk_select_label(ctx, items[i], NK_TEXT_LEFT, ui_state.selected_item == i)) {
                ui_state.selected_item = i;
            }
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Background Color:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(ui_state.background_color),
            nk_vec2(nk_widget_width(ctx), 400))) {
            nk_layout_row_dynamic(ctx, 120, 1);
            ui_state.background_color = nk_color_picker(ctx, ui_state.background_color, NK_RGBA);
            nk_combo_end(ctx);
        }
    }
    nk_end(ctx);

    // Main Content Area
    if (nk_begin(ctx, "Content", nk_rect(250, 30, 1420, 670),
        NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Main Content Area", NK_TEXT_CENTERED);
        
        // Display some state info
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Slider Value: %d", ui_state.slider_value);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        
        snprintf(buffer, sizeof(buffer), "Selected Item: %d", ui_state.selected_item);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        
        // Fixed buffer overflow warning by limiting string length
        snprintf(buffer, sizeof(buffer), "Input Text: %.200s%s", 
                ui_state.text, 
                strlen(ui_state.text) > 200 ? "..." : "");
        nk_label(ctx, buffer, NK_TEXT_LEFT);
    }
    nk_end(ctx);

    // About Dialog (Modal)
    if (ui_state.show_about) {
        if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About",
            NK_WINDOW_CLOSABLE, nk_rect(760, 300, 400, 200))) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "RoofNut Editor", NK_TEXT_CENTERED);
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "A powerful editing tool", NK_TEXT_CENTERED);
            nk_label(ctx, "Version 1.0", NK_TEXT_CENTERED);
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Close")) {
                ui_state.show_about = false;
                nk_popup_close(ctx);
            }
            nk_popup_end(ctx);
        } else {
            ui_state.show_about = false;
        }
    }

    RenderImage("--insert your image path--", 1920, 1080, 0.5, -0.5);


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
