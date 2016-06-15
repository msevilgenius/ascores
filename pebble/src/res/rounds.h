#pragma once

struct round_cat{
	char name[32];
	uint8_t num_rounds;
	uint32_t rounds_keys[12];
};

struct round_data{
	uint16_t ends;
	uint16_t arrows_per_end;
	bool imperial;
	char name[32];
};