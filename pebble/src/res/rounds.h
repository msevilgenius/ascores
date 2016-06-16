#pragma once

#define MAX_ENDS 144

typedef struct round_list{
	uint8_t num_rounds;
	uint32_t rounds_keys[100];
}RoundList;

typedef struct round_data{
	uint8_t ends;
	uint8_t arrows_per_end;
	bool imperial;
	char name[32];
}RoundData;