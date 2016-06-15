#pragma once

struct round_cat{
	char[32] name;
	uint8_t num_rounds;
	uint32_t[12] rounds_keys;
};

struct round_data{
	uint16_t hdozens;
	uint16_t ends;
	uint16_t arrows_per_end;
	bool imperial;
	char[32] name;
};