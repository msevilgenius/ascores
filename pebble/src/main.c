#include <pebble.h>

#include "windows/score_entry.h"
#include "windows/setup.h"
#include "windows/main_window.h"



static void init() {
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
