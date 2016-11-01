#include "score_view.h"

static Window *score_view_window;
static TextLayer *total_label,
				 *hits_label,
				 *golds_label,
				 *ave_end_label,
				 *ave_arrow_label,
				 *total_val_text,
				 *hits_val_text,
				 *golds_val_text,
				 *ave_end_val_text,
				 *ave_arrow_val_text;

static RoundData *s_round;
static uint8_t *s_scores;
static uint16_t s_total;
static uint8_t s_hits, s_golds, ave_end, ave_arrow;
static char s_total_text[5], s_hits_text[4], s_golds_text[4], ave_end_text[3], ave_arrow_text[3];

static void calc_values() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "calcing scores");
	uint16_t num_arrows = s_round->ends * s_round->arrows_per_end;
	uint8_t end_total;
	uint8_t arrow_score;
	
	s_total = 0;
	s_golds = 0;
	s_hits = num_arrows;
	ave_end = 0;
	ave_arrow = 0;
	
	for (uint8_t end = 0; end < s_round->ends; ++end){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "calcing end %u", end);
		end_total = 0;
		for (uint8_t arrow = 0; arrow < s_round->arrows_per_end; ++arrow){
			arrow_score = s_scores[end * s_round->arrows_per_end + arrow] & 0x7F;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "arrow: %u", arrow_score);
			end_total += arrow_score;
			if (arrow_score > 9) { ++s_golds; }
			if (arrow_score == 0) { --s_hits; }
		}
		s_total += end_total;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "end total %u", end_total);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "total so far %u", s_total);
	}
	
	ave_arrow = s_total / num_arrows;
	ave_end = s_total / s_round->ends;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ave_arrow %u", ave_arrow);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ave_end %u", ave_end);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "total %u", s_total);
	snprintf(s_total_text, 5, "%u", s_total);
	snprintf(s_hits_text, 4, "%u", s_hits);
	snprintf(s_golds_text, 4, "%u", s_golds);
	snprintf(ave_end_text, 4, "%u", ave_end);
	snprintf(ave_arrow_text, 4, "%u", ave_arrow);
}

static void score_view_window_load(Window* window) {

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

// Labels
	APP_LOG(APP_LOG_LEVEL_DEBUG, "creating labels");	
	total_label = text_layer_create(GRect(4, 12, bounds.size.w / 2, 32));
	text_layer_set_background_color(total_label, GColorClear);
	text_layer_set_text_color(total_label, GColorBlack);
	text_layer_set_text(total_label, "Total:");
	text_layer_set_font(total_label, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(total_label, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(total_label));

	hits_label = text_layer_create(GRect(4, 48, bounds.size.w / 2, 22));
	text_layer_set_background_color(hits_label, GColorClear);
	text_layer_set_text_color(hits_label, GColorBlack);
	text_layer_set_text(hits_label, "Hits:");
	text_layer_set_font(hits_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(hits_label, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(hits_label));

	golds_label = text_layer_create(GRect(4, 72, bounds.size.w / 2, 22));
	text_layer_set_background_color(golds_label, GColorClear);
	text_layer_set_text_color(golds_label, GColorBlack);
	text_layer_set_text(golds_label, "Golds:");
	text_layer_set_font(golds_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(golds_label, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(golds_label));

	ave_end_label = text_layer_create(GRect(4, 96, bounds.size.w / 2, 22));
	text_layer_set_background_color(ave_end_label, GColorClear);
	text_layer_set_text_color(ave_end_label, GColorBlack);
	text_layer_set_text(ave_end_label, "Ave. End:");
	text_layer_set_font(ave_end_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(ave_end_label, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(ave_end_label));

	ave_arrow_label = text_layer_create(GRect(4, 120, bounds.size.w / 2, 22));
	text_layer_set_background_color(ave_arrow_label, GColorClear);
	text_layer_set_text_color(ave_arrow_label, GColorBlack);
	text_layer_set_text(ave_arrow_label, "Ave. Arrow:");
	text_layer_set_font(ave_arrow_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(ave_arrow_label, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(ave_arrow_label));
// values
	APP_LOG(APP_LOG_LEVEL_DEBUG, "creating vals");
	APP_LOG(APP_LOG_LEVEL_DEBUG, "total");
	total_val_text = text_layer_create(GRect(bounds.size.w / 2, 12, (bounds.size.w / 2) - 12, 32));
	text_layer_set_background_color(total_val_text, GColorClear);
	text_layer_set_text_color(total_val_text, GColorBlack);
	text_layer_set_text(total_val_text, s_total_text);
	text_layer_set_font(total_val_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(total_val_text, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(total_val_text));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "hits");
	hits_val_text = text_layer_create(GRect(bounds.size.w / 2, 44, (bounds.size.w / 2) - 12, 24));
	text_layer_set_background_color(hits_val_text, GColorClear);
	text_layer_set_text_color(hits_val_text, GColorBlack);
	text_layer_set_text(hits_val_text, s_hits_text);
	text_layer_set_font(hits_val_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(hits_val_text, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(hits_val_text));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "golds");
	golds_val_text = text_layer_create(GRect(bounds.size.w / 2, 68, (bounds.size.w / 2) - 12, 24));
	text_layer_set_background_color(golds_val_text, GColorClear);
	text_layer_set_text_color(golds_val_text, GColorBlack);
	text_layer_set_text(golds_val_text, s_golds_text);
	text_layer_set_font(golds_val_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(golds_val_text, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(golds_val_text));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "ave end");
	ave_end_val_text = text_layer_create(GRect(bounds.size.w / 2, 92, (bounds.size.w / 2) - 12, 24));
	text_layer_set_background_color(ave_end_val_text, GColorClear);
	text_layer_set_text_color(ave_end_val_text, GColorBlack);
	text_layer_set_text(ave_end_val_text, ave_end_text);
	text_layer_set_font(ave_end_val_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(ave_end_val_text, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(ave_end_val_text));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "ave arrow");
	ave_arrow_val_text = text_layer_create(GRect(bounds.size.w / 2, 116, (bounds.size.w / 2) - 12, 24));
	text_layer_set_background_color(ave_arrow_val_text, GColorClear);
	text_layer_set_text_color(ave_arrow_val_text, GColorBlack);
	text_layer_set_text(ave_arrow_val_text, ave_arrow_text);
	text_layer_set_font(ave_arrow_val_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(ave_arrow_val_text, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(ave_arrow_val_text));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "done");
}

static void score_view_window_unload(Window* window) {
	text_layer_destroy(total_label);
	text_layer_destroy(hits_label);
	text_layer_destroy(golds_label);
	text_layer_destroy(ave_end_label);
	text_layer_destroy(ave_arrow_label);
	text_layer_destroy(total_val_text);
	text_layer_destroy(hits_val_text);
	text_layer_destroy(golds_val_text);
	text_layer_destroy(ave_end_val_text);
	text_layer_destroy(ave_arrow_val_text);
}

void score_view_create(RoundData *round, uint8_t *scores) {
	
	s_round = round;
	s_scores = scores;
	calc_values();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "calc'd vals");
	
	score_view_window = window_create();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "created window");
	window_set_window_handlers(score_view_window, (WindowHandlers) {
		.load = score_view_window_load,
		.unload = score_view_window_unload
	});
	APP_LOG(APP_LOG_LEVEL_DEBUG, "pushing window");
	window_stack_push(score_view_window, true);
}