#pragma once
#include <pebble.h>

#include "res/rounds.h"

typedef void (*round_set_cb)(RoundData);

void rsel_init(round_set_cb select_callback);
void rsel_deinit();