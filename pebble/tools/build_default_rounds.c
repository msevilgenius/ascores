#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "res/rounds.h"


int write_data(const char *fname, const RoundData *data, const size_t count) {
	FILE* ofile = fopen(fname, "wb");
	if (!ofile){
		return (-1);
	}
	size_t rv = fwrite(data, sizeof(RoundData), count, ofile);
	if (rv != count){
		return (-1);
	}
	return fclose(ofile);
}

RoundData *read_input(const char *fname, const size_t count) {
	RoundData *rounds_bin = malloc(sizeof(RoundData) * count);
	char buf[32];
	if (!rounds_bin){
		return NULL;
	}
	FILE* ifile = fopen(fname, "r");
	for (size_t i = 0; i < count; ++i){
		fgets(buf, sizeof(buf), ifile);
		for (size_t c = 0; c < sizeof(buf); ++c){
			if (buf[c] == '\n'){
				buf[c] = '\0';
				break;
			}
		}
		memcpy(rounds_bin[i].name, buf, sizeof(buf));
		fgets(buf, sizeof(buf), ifile);
		rounds_bin[i].arrows_per_end = atoi(buf);
		fgets(buf, sizeof(buf), ifile);
		rounds_bin[i].ends = atoi(buf);
		fgets(buf, sizeof(buf), ifile);
		rounds_bin[i].imperial = atoi(buf);
	}

	fclose(ifile);

	return rounds_bin;
}

int main(int argc, char **argv) {
	RoundData *rounds_bin;
	uint32_t r_count;
	const char *in_name, *out_name;
	
	if (argc != 4){
		printf("usage:\n %s <input_file> <round_count> <out_file>", argv[0]);
		return 0;
	}
	
	in_name = argv[1];
	out_name = argv[3];
	r_count = atoi(argv[2]);
	
	rounds_bin = read_input(in_name, r_count);
	return write_data(out_name, rounds_bin, r_count);
}