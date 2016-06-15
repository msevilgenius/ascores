#include "setup.h"
#include "round_select.h"
#include "res/rounds.h"
#include "strings.h"

static Window *s_setup_window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *status_bar;

static struct round_data s_round;


static uint16_t menu_get_num_sections_cb(MenuLayer *ml, void *data) {
    return 1;
}

static uint16_t menu_get_num_rows_cb(MenuLayer *ml, uint16_t section, void *data) {
    return 4;
}

static void menu_draw_row_cb(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    
    char buf[8];
    // TODO fill in subtitle values
    switch (cell_index->row){
        case 0:
            // Round
            menu_cell_basic_draw(ctx, cell_layer, SETUP_ROUND, s_round.name, NULL);
            break;
        case 1:
            // ApE
            snprintf(buf, 8, "%d", s_round.arrows_per_end);
            menu_cell_basic_draw(ctx, cell_layer, SETUP_APE, buf, NULL);
            break;
        case 2:
            // Ends
            snprintf(buf, 8, "%d", s_round.ends);
            menu_cell_basic_draw(ctx, cell_layer, SETUP_ENDS, buf, NULL);
            break;
        case 3:
            // Scoring Zones
            snprintf(buf, 8, "%d-zone", (s_round.imperial ? 5 : 10));
            menu_cell_basic_draw(ctx, cell_layer, SETUP_SZONES, buf, NULL);
            break;
    }
    
}

static void menu_select_cb(MenuLayer *ml, MenuIndex *cell_index, void *data) {
    
    switch (cell_index->row){
        case 0:
            // Round
            break;
        case 1:
            // ApE
            break;
        case 2:
            // Ends
            break;
        case 3:
            // Scoring Zones
            // toggle between 5- and 10-zone
            s_round.imperial = !s_round.imperial;
            // TODO change name to int type andget names from storage, 0 = custom 
            //s_round.name = "Custom"
            break;
    }

    menu_layer_reload_data(ml);

}

static void setup_window_load(Window *window) {
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

static void setup_window_unload(Window *window) {
    menu_layer_destroy(s_menu_layer);
}

void setup_init() {
    // TODO read cached round
    s_round.ends = 12;
    s_round.arrows_per_end = 3;
    s_round.imperial = true;
    strncpy(s_round.name, "Custom", 32);

    s_setup_window = window_create();

    window_set_window_handlers(s_setup_window, (WindowHandlers) {
        .load = setup_window_load,
        .unload = setup_window_unload
    });

    window_stack_push(s_setup_window, true);
}

void setup_deinit() {
    window_destroy(s_setup_window);
}