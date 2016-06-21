#include "score_entry.h"
#include "modules/storage.h"
#include "score_view.h"

typedef struct{
	uint8_t arrow_in_end;
	uint8_t curr_end;
}Progress;

uint8_t *scores;
uint8_t *end_scores;
Progress progress;
static RoundData round_data;


static Window *score_entry_window;

static GBitmap *entry_bg_img;
static BitmapLayer *entry_bg_layer;
static TextLayer *num_entry_layer[3];
static TextLayer *progress_text_layer;
static StatusBarLayer *status_bar;
static Layer *selection_layer;
static GBitmap *arrow_up_img, *arrow_down_img,
			   *arrow_up_o_img, *arrow_down_o_img,
			   *box_o_img;
static char progress_text[28];

static void update_progress_text() {
	snprintf(progress_text, 28, "Arrow: %u/%u\nEnd: %u/%u",
				progress.arrow_in_end + 1, (round_data.arrows_per_end),
				progress.curr_end + 1, (round_data.ends));
	text_layer_set_text(progress_text_layer, progress_text);
}

static void save_progress() {
	persist_write_data(PS_CURR_SCORES, (void*) scores,
						progress.arrow_in_end * progress.curr_end);
	persist_write_data(PS_CURR_PROGRESS, (void*) &progress, sizeof(Progress));
}

static void load_progress() {
	APP_LOG(APP_LOG_LEVEL_INFO, "load prog called");
	persist_read_data(PS_CURR_ROUND, (void*) &round_data, sizeof(RoundData));
	APP_LOG(APP_LOG_LEVEL_INFO, "loaded curr round");
	persist_read_data(PS_CURR_PROGRESS, (void*) &progress, sizeof(Progress));
	APP_LOG(APP_LOG_LEVEL_INFO, "loaded progress");
	APP_LOG(APP_LOG_LEVEL_INFO, "rd_ape: %u, p_ce: %u", round_data.arrows_per_end, progress.curr_end);
	scores = malloc(sizeof(uint8_t) * round_data.ends * round_data.arrows_per_end);
	APP_LOG(APP_LOG_LEVEL_INFO, "malloc'd scores");
	persist_read_data(PS_CURR_SCORES, (void*) scores,
						round_data.arrows_per_end * progress.curr_end);
	APP_LOG(APP_LOG_LEVEL_INFO, "loaded curr scores");
}

static void update_current_score_entry_text() {
	text_layer_set_text(num_entry_layer[progress.arrow_in_end % 3], score2str(end_scores[progress.arrow_in_end]));
}

static void move_selection_box_animation_end(Animation *animation, bool finished, void *ctx) {
	animation_destroy(animation);
}

static void move_selection_box_animated(uint8_t from_pos, uint8_t to_pos) {

	const int duration_ms = 200;
	GRect start, finish;
	start = GRect(10 + from_pos * 41, 93, 42, 60);
	finish = GRect(10 + to_pos * 41, 93, 42, 60);
	
	PropertyAnimation *prop_anim = property_animation_create_layer_frame(selection_layer,
																		&start, &finish);
	Animation *anim = property_animation_get_animation(prop_anim);
	
	animation_set_duration(anim, duration_ms);
	animation_set_handlers(anim,(AnimationHandlers) {
		.stopped = move_selection_box_animation_end
	}, NULL);
	
	animation_schedule(anim);
}

static void clear_score_entry_text() {
	// fuck looping for this
	text_layer_set_text(num_entry_layer[0], "-");
	text_layer_set_text(num_entry_layer[1], "-");
	text_layer_set_text(num_entry_layer[2], "-");
}

static void button_up_handler(ClickRecognizerRef recognizer, void *ctx) {
	if (!round_data.imperial && end_scores[progress.arrow_in_end] < 11){
		end_scores[progress.arrow_in_end] += 1;
	}else if(end_scores[progress.arrow_in_end] == 0){ // imperial && score = M
		end_scores[progress.arrow_in_end] = 1;
	}else if(end_scores[progress.arrow_in_end] < 8){ // imperial && score = [1357]
		end_scores[progress.arrow_in_end] += 2;
	}
	update_current_score_entry_text();
}

static void button_dn_handler(ClickRecognizerRef recognizer, void *ctx) {
	if (!round_data.imperial && end_scores[progress.arrow_in_end] > 0){
		end_scores[progress.arrow_in_end] -= 1;
	}else if(end_scores[progress.arrow_in_end] > 2){ // imperial && score = [3579]
		end_scores[progress.arrow_in_end] -= 2;
	}else if(end_scores[progress.arrow_in_end] > 0){ // imperial && score = 1
		end_scores[progress.arrow_in_end] = 0;
	}
	update_current_score_entry_text();
}

static void button_select_handler(ClickRecognizerRef recognizer, void *ctx) {
	uint8_t prev_arrow_in_end = progress.arrow_in_end;
	if (++progress.arrow_in_end >= round_data.arrows_per_end){
		// finished end
		progress.arrow_in_end = 0;
		// TODO save scores to storage
		// commit end scores and reset
		for (uint8_t i = 0; i < round_data.arrows_per_end; ++i){
			scores[ (progress.curr_end*round_data.arrows_per_end) + i] = score2dat(end_scores[i]);
			end_scores[i] = 0x80;
		}
		if (++progress.curr_end >= round_data.ends){
			// finished shoot
			// TODO send score to phone, close scoresheet, make unresumeable, etc. otherwise this can crash
			save_progress();
			window_stack_pop(false);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "showing scores");
			score_view_create(&round_data, scores);
			return;
		}else{
			// starting next end, set first score to highest
			// FUTURE_TODO set to ave. first arrow?
			end_scores[0] = round_data.imperial ? 9 : 10;
			clear_score_entry_text();
			update_current_score_entry_text();
			save_progress();
		}
	}else{ // just move on to next arrow
		// set next score to prev score (as it should only ever by <= to it)
		end_scores[progress.arrow_in_end] = end_scores[progress.arrow_in_end - 1];
		if (progress.arrow_in_end % 3 == 0){
			clear_score_entry_text();
		}
		update_current_score_entry_text();
	}
	update_progress_text();
	move_selection_box_animated(prev_arrow_in_end % 3, progress.arrow_in_end % 3);
}

static void config_provider(Window *window) {
	window_single_repeating_click_subscribe(BUTTON_ID_UP, 150, button_up_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 150, button_dn_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, button_select_handler);
}

static void selection_layer_update_proc(Layer *layer, GContext *ctx) {
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	GRect bitmap_bounds;
	bitmap_bounds = gbitmap_get_bounds(arrow_up_img);
	graphics_draw_bitmap_in_rect(ctx, arrow_up_img,
					GRect((42 - bitmap_bounds.size.w)/2, 0, bitmap_bounds.size.w, bitmap_bounds.size.h));
	graphics_draw_bitmap_in_rect(ctx, arrow_down_img,
					GRect((42 - bitmap_bounds.size.w)/2, 60 - bitmap_bounds.size.h,
					bitmap_bounds.size.w, bitmap_bounds.size.h));
	
	bitmap_bounds = gbitmap_get_bounds(box_o_img);
	graphics_draw_bitmap_in_rect(ctx, box_o_img,
					GRect(0, 9, bitmap_bounds.size.w, bitmap_bounds.size.h));
}

static void window_load(Window* window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	entry_bg_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_SCORE_BG);
	entry_bg_layer = bitmap_layer_create(GRect(12, 104, 120, 38));

	bitmap_layer_set_compositing_mode(entry_bg_layer, GCompOpSet);
	bitmap_layer_set_bitmap(entry_bg_layer, entry_bg_img);

	layer_add_child(window_layer, bitmap_layer_get_layer(entry_bg_layer));
	
	arrow_up_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_ARROW_UP);
	arrow_down_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_ARROW_DN);
	arrow_up_o_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_ARROW_UP_OUTER);
	arrow_down_o_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_ARROW_DN_OUTER);
	box_o_img = gbitmap_create_with_resource(RESOURCE_ID_IMG_BOX_OUTER);
	
	selection_layer = layer_create(GRect(10, 93, 42, 60));
	layer_set_update_proc(selection_layer, selection_layer_update_proc);
	layer_add_child(window_layer, selection_layer);

	for (uint8_t i = 0; i < 3; ++i){
		num_entry_layer[i] = text_layer_create(
			GRect((12+i*41), 104, 38, 38));
	
		text_layer_set_background_color(num_entry_layer[i], GColorClear);
		text_layer_set_text_color(num_entry_layer[i], GColorBlack);
		text_layer_set_font(num_entry_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		text_layer_set_text_alignment(num_entry_layer[i], GTextAlignmentCenter);
	
		layer_add_child(window_layer, text_layer_get_layer(num_entry_layer[i]));
	}

	progress_text_layer = text_layer_create(
		GRect(10, PBL_IF_ROUND_ELSE(20, 16), bounds.size.w - 20, 48));

	text_layer_set_background_color(progress_text_layer, GColorClear);
	text_layer_set_text_color(progress_text_layer, GColorBlack);
	text_layer_set_font(progress_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(progress_text_layer,
		PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
	update_progress_text();

	layer_add_child(window_layer, text_layer_get_layer(progress_text_layer));
	
	status_bar = status_bar_layer_create();
	layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
	
	
	window_set_click_config_provider(score_entry_window, (ClickConfigProvider) config_provider);

	clear_score_entry_text();
	update_current_score_entry_text();
}

static void window_unload(Window* window) {
	
	status_bar_layer_destroy(status_bar);
	
	for (uint8_t i = 0; i < 3; ++i){
		text_layer_destroy(num_entry_layer[i]);
	}
	text_layer_destroy(progress_text_layer);

	 gbitmap_destroy(entry_bg_img);
	 bitmap_layer_destroy(entry_bg_layer);
	 
	 gbitmap_destroy(arrow_up_img);
	 gbitmap_destroy(arrow_down_img);
	 gbitmap_destroy(arrow_up_o_img);
	 gbitmap_destroy(arrow_down_o_img);
	 gbitmap_destroy(box_o_img);
	 
	 layer_destroy(selection_layer);
	 score_entry_destroy();
}

void score_entry_create (RoundData *round) {
	memcpy(&round_data, round, sizeof(RoundData));
	// reset stored progress to prevent incorrect resuming
	persist_delete(PS_CURR_PROGRESS);
	persist_delete(PS_CURR_SCORES);
	scores = malloc(sizeof(uint8_t) * round_data.ends * round_data.arrows_per_end);
	end_scores = malloc(sizeof(uint8_t) * round_data.arrows_per_end);
	progress.arrow_in_end = 0;
	progress.curr_end = 0;
	end_scores[0] = round_data.imperial ? 9 : 10;
	for (uint8_t i = 1; i < round_data.arrows_per_end; ++i){
		end_scores[i] = 0x80;
	}
	
	score_entry_window = window_create();

	window_set_window_handlers(score_entry_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});

	window_stack_push(score_entry_window, true);

}

void score_entry_resume() {
	APP_LOG(APP_LOG_LEVEL_INFO, "resume called");
	load_progress();
	APP_LOG(APP_LOG_LEVEL_INFO, "loaded prog");
	progress.arrow_in_end = 0;
	APP_LOG(APP_LOG_LEVEL_INFO, "reset arrow_in_end");
	end_scores = malloc(sizeof(uint8_t) * round_data.arrows_per_end);
	APP_LOG(APP_LOG_LEVEL_INFO, "malloc'd end scores");
	end_scores[0] = round_data.imperial ? 9 : 10;
	for (uint8_t i = 1; i < round_data.arrows_per_end; ++i){
		end_scores[i] = 0x80;
	}
	APP_LOG(APP_LOG_LEVEL_INFO, "init'd end scores");
	
	score_entry_window = window_create();
	APP_LOG(APP_LOG_LEVEL_INFO, "created window");

	window_set_window_handlers(score_entry_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});

	window_stack_push(score_entry_window, true);
}

void score_entry_destroy() {
	// TODO put in window unload so leaks prevented
	if (scores){
		// TODO save scores (?)
		free(scores);
		scores = NULL;
	}
	
	if (score_entry_window) {
		window_destroy(score_entry_window);
		score_entry_window = NULL;
	}
}