// Ethan Piper
// ME333
// utilities.c

#include "utilities.h"

enum mode_types mode;


void set_mode(int mode_1) {
	mode = mode_1;
}

int get_mode() {
	return mode;
}