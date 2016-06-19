#include "storage.h"
#include "res/rounds.h"

bool storage_init(bool force) {
	if (force || !persist_exists(PS_CURR_ROUND)){
		// first run
		storage_reset_current();
		return true;
	}
	return false;
}

void storage_reset_current() {
	persist_delete(PS_CURR_PROGRESS);
	persist_delete(PS_CURR_SCORES);
	RoundData round = {
		.ends = 12,
		.arrows_per_end = 6,
		.imperial = false,
		.name = "Default"
	};
	persist_write_data(PS_CURR_ROUND, (void*) &round, sizeof(RoundData));
}

bool storage_has_curr_round() {
	return persist_exists(PS_CURR_PROGRESS) && persist_exists(PS_CURR_ROUND);
}

