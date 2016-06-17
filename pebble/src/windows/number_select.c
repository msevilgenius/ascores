#include "number_select.h"


static Window *s_numsel_window;
static TextLayer *text_number;
static TextLayer *text_title;
static ActionBarLayer *action_bar;
static GBitmap *bitmap_plus, *bitmap_minus, *bitmap_tick;

static numsel_found_number_callback num_cb;
static void *cb_arg;

static const char *s_title;

static uint16_t number;
static uint16_t num_min;
static uint16_t num_max;

void update_number_display() {
    static char buf[5];
    snprintf(buf, 5, "%d", number);
    text_layer_set_text(text_number, buf);
    APP_LOG(APP_LOG_LEVEL_INFO, "update display to %s", buf);
}
    
void button_up_handler(ClickRecognizerRef recognizer, void *ctx) {
    if (++number >= num_max) {
        number = num_max;
        action_bar_layer_clear_icon(action_bar, BUTTON_ID_UP);
    }
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, bitmap_minus, true);
    update_number_display();
    APP_LOG(APP_LOG_LEVEL_INFO, "incr");
}

void button_dn_handler(ClickRecognizerRef recognizer, void *ctx) {
    if (--number <= num_min) {
        number = num_min;
        action_bar_layer_clear_icon(action_bar, BUTTON_ID_DOWN);
    }
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, bitmap_plus, true);
    update_number_display();
    APP_LOG(APP_LOG_LEVEL_INFO, "decr");
}

void button_select_handler(ClickRecognizerRef recognizer, void *ctx) {
    numsel_destroy();
    num_cb(number, cb_arg);
    APP_LOG(APP_LOG_LEVEL_INFO, "select");
}

void config_provider(Window *window) {
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 150, button_up_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 150, button_dn_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, button_select_handler);
}

static void numsel_window_load(Window *window) {
    
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    bounds.size.w -= ACTION_BAR_WIDTH;
    
    bitmap_plus = gbitmap_create_with_resource(RESOURCE_ID_IMG_PLUS_16);
    bitmap_minus = gbitmap_create_with_resource(RESOURCE_ID_IMG_MINUS_16);
    bitmap_tick = gbitmap_create_with_resource(RESOURCE_ID_IMG_TICK_16);
    
    action_bar = action_bar_layer_create();
    action_bar_layer_add_to_window(action_bar, window);
    action_bar_layer_set_click_config_provider(action_bar,
                                (ClickConfigProvider) config_provider);
    if (number < num_max)
        action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, bitmap_plus, true);
    if (number > num_min)
        action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, bitmap_minus, true);
    action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, bitmap_tick, true);

    text_title = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(28, 22), bounds.size.w, 48));

    text_layer_set_background_color(text_title, GColorClear);
    text_layer_set_text_color(text_title, GColorBlack);
    text_layer_set_text(text_title, s_title);
    text_layer_set_font(text_title, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text_alignment(text_title, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(text_title));
    
    
    text_number = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(68, 62), bounds.size.w, 64));

    text_layer_set_background_color(text_number, GColorClear);
    text_layer_set_text_color(text_number, GColorBlack);
    text_layer_set_font(text_number, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
    text_layer_set_text_alignment(text_number, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(text_number));
    update_number_display();
    APP_LOG(APP_LOG_LEVEL_INFO, "loaded numsel window");
}

static void numsel_window_unload(Window *window) {
    text_layer_destroy(text_title);
    text_layer_destroy(text_number);
    
    gbitmap_destroy(bitmap_plus);
    gbitmap_destroy(bitmap_minus);
    gbitmap_destroy(bitmap_tick);
}

void numsel_create(uint16_t init_num, uint16_t min, uint16_t max, const char *title) {
	
	num_min = min;
	num_max = max;
	number = init_num;
    
    s_title = title; 
	
    s_numsel_window = window_create();

    window_set_window_handlers(s_numsel_window, (WindowHandlers) {
        .load = numsel_window_load,
        .unload = numsel_window_unload
    });

}

void numsel_set_done_callback(numsel_found_number_callback cb, void *arg) {
    num_cb = cb;
    cb_arg = arg;
}

void numsel_push() {
    window_stack_push(s_numsel_window, true);
}

void numsel_destroy() {
    window_stack_pop(true);
    window_destroy(s_numsel_window);
}