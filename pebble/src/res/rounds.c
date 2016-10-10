#include "rounds.h"

const char* score2str(uint8_t score) {
	switch (score){
		case 0:
			return "M";
		case 1:
			return "1";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		case 7:
			return "7";
		case 8:
			return "8";
		case 9:
			return "9";
		case 10:
			return "10";
		case 11:
			return "X";
		default:
			return "-";
	}
}

uint8_t dat2score(uint8_t dat){
	if (dat == (10 | 0b10000000)){
		return 11;
	}
	return dat;
}

uint8_t score2dat(uint8_t score){
	if (score == 11){
		return 10 | 0b10000000;
	}
	return score;
}