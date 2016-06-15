#pragma once
#include <pebble.h>

typedef void (*numsel_found_number_callback)(uint16_t, void*);


void numsel_create(uint16_t init_num, uint16_t min, uint16_t max, const char *title);

void numsel_set_done_callback(numsel_found_number_callback cb, void *arg);

void numsel_push();

void numsel_destroy();