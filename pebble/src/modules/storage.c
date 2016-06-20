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

void init_rounds(bool force){
	if (force || !persist_exists(PS_ROUNDS_TABLE)){
		uint32_t *rounds_table;
		RoundData round;
		ResHandle rhandle = resource_get_handle(RESOURCE_ID_DEFAULT_ROUNDS);
		size_t res_size = resource_size(rhandle);
		size_t count = res_size / sizeof(RoundData);
		rounds_table = malloc(sizeof(uint32_t) * count);

		for (size_t i = 0; i < count; ++i){
			resource_load_byte_range(rhandle, sizeof(RoundData) * i,
												(uint8_t*) &round, sizeof(RoundData));
			persist_write_data(PS_ROUNDS_START + i, (void*) &round, sizeof(RoundData));
			rounds_table[i] = PS_ROUNDS_START + i;
		}
		persist_write_data(PS_ROUNDS_TABLE, (void*) rounds_table, sizeof(uint32_t) * count);
	}
}
