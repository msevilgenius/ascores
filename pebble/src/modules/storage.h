#pragma once
#include <pebble.h>

#define PS_CURR_ROUND 0
#define PS_CURR_PROGRESS 1
#define PS_CURR_SCORES 2

#define PS_ROUNDS_TABLE 4
#define PS_ROUNDS_START 11

#define PS_PAST_SCORES_TABLE_START 5
#define PS_PAST_SCORES_TABLE_LENGTH 6
// 11+ are free for rounds

bool storage_init(bool force);

void storage_reset_current();

bool storage_has_curr_round();

void init_rounds(bool force);
