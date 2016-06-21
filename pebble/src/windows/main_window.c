#include "main_window.h"
#include "setup.h"
#include "score_entry.h"
#include "strings.h"
#include "modules/storage.h"
#include "score_view.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *status_bar;

static bool can_resume() {
    return storage_has_curr_round();
}

static uint16_t menu_get_num_sections_cb(MenuLayer *ml, void *data) {
    return 1;
}

static uint16_t menu_get_num_rows_cb(MenuLayer *ml, uint16_t section, void *data) {
    return 1 + (can_resume() ? 1 : 0);
}

static void menu_draw_row_cb(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    uint8_t row = cell_index->row + (can_resume() ? 0 : 1);
    switch (row){
        case 0:
            // resume
            menu_cell_basic_draw(ctx, cell_layer, MAIN_RESUME, "current_progress", NULL);
            break;
        case 1:
            // new
            menu_cell_title_draw(ctx, cell_layer, MAIN_NEW);
            break;
    }
    
}

static void menu_select_cb(MenuLayer *ml, MenuIndex *cell_index, void *data) {
    uint8_t row = cell_index->row + (can_resume() ? 0 : 1);
    switch (row){
        case 0:
            // resume - to score_entry (resume)
            score_entry_resume();
            break;
        case 1:
            // new - to screen setup
            setup_init();
            break;
    }
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    
    status_bar = status_bar_layer_create();
    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
    
    GRect bounds = layer_get_frame(window_layer);
    bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;
    bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
    s_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = menu_get_num_sections_cb,
        .get_num_rows = menu_get_num_rows_cb,
        .draw_row = menu_draw_row_cb,
        .select_click = menu_select_cb,
    });
    
    menu_layer_set_click_config_onto_window(s_menu_layer, window);
    
    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
    menu_layer_destroy(s_menu_layer);
}

void main_init() {
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(s_main_window, true);
}

void main_deinit() {
    window_destroy(s_main_window);
}


