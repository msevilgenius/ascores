#include <pebble.h>

#include "windows/score_entry.h"
#include "windows/setup.h"
#include "windows/main_window.h"
#include "modules/storage.h"



static void init() {
	if (storage_init(false)){ // if firstrun
	}
	init_rounds(true);
	main_init();
}

static void deinit() {
	main_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
