#include "round_select.h"
#include "strings.h"
#include "modules/storage.h"

static Window *s_rsel_window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *status_bar;

static uint32_t round_count;
static uint32_t *rounds_table;
static RoundData *rounds_cache;
static round_set_cb r_select_cb;

static uint16_t menu_get_num_sections_cb(MenuLayer *ml, void *data) {
	return 1;
}

static uint16_t menu_get_num_rows_cb(MenuLayer *ml, uint16_t section, void *data) {
	return round_count;
}

// menu will display rounds wich have been setup in app
// if cant connect to phone, load from storage
// if none in storage show message to say 'get predefined rounds from app'
 
static void menu_draw_row_cb(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

	char doz_str[4];
	uint8_t arrows = rounds_cache[cell_index->row].arrows_per_end * rounds_cache[cell_index->row].ends;
	uint8_t doz = arrows / 12;
	if (arrows % 12 == 0){
		snprintf(doz_str, 4, "%u", doz);
	}else{
		snprintf(doz_str, 4, "%u.5", doz);
	}
	menu_cell_basic_draw(ctx, cell_layer, rounds_cache[cell_index->row].name, doz_str, NULL);
}

static void menu_select_cb(MenuLayer *ml, MenuIndex *cell_index, void *data) {
	window_stack_pop(true);
	r_select_cb(rounds_cache[cell_index->row]);
}

static void rsel_window_load(Window *window) {
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

	round_count = persist_get_size(PS_ROUNDS_TABLE) / sizeof(uint32_t);
	rounds_table = malloc(sizeof(uint32_t) * round_count);
	persist_read_data(PS_ROUNDS_TABLE, (void*) rounds_table, sizeof(uint32_t) * round_count);
	rounds_cache = malloc(sizeof(RoundData) * round_count);
	for (size_t i = 0; i < round_count; ++i){
		persist_read_data(rounds_table[i], (void*) &(rounds_cache[i]), sizeof(RoundData));
	}
	
    menu_layer_reload_data(s_menu_layer);
}

static void rsel_window_unload(Window *window) {
	free(rounds_table);
	free(rounds_cache);
	menu_layer_destroy(s_menu_layer);
	status_bar_layer_destroy(status_bar);
}

void rsel_init(round_set_cb select_callback) {
	r_select_cb = select_callback;
	s_rsel_window = window_create();

	window_set_window_handlers(s_rsel_window, (WindowHandlers) {
		.load = rsel_window_load,
		.unload = rsel_window_unload
	});
	window_stack_push(s_rsel_window, true);
}

void rsel_deinit() {
	window_destroy(s_rsel_window);
}