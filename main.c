#include <stdio.h>
#include <stdlib.h>
#include "motor.h"

int main(int argc, char **argv) {
	int left, right;
	if(argc < 3) {
		fprintf(stderr, "Usage: %s [-100 - 100] [-100 - 100]\n"
				"    Sets the left and right motor speeds.\n", argv[0]);
		return 1;
	}

	left  = strtol(argv[1], NULL, 0);
	right = strtol(argv[2], NULL, 0);
	
	if(left > 100)
		left = 100;
	if(right > 100)
		right = 100;
	if(left < -100)
		left = -100;
	if(right < -100)
		right = -100;

	pwm_init();
	pwm_left(left);
	pwm_right(right);
	return 0;
}
