/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 *
 * Desc:    Functions for driving the Traxxas E-MAXX RC truck.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "motor.h"

void left_motor(int percent) {
	int speed;
	if(percent<-100)
		percent=-100;
	if(percent>100)
		percent=100;

	if(percent > -10 && percent < 10)
		percent = 0;

	// -100% => PWM_MAX_REVERSE_SPEED
	//  100% => PWM_MAX_FORWARD_SPEED
	//    0% => PWM_IDLE_STOP
	if(percent)
		speed = PWM_IDLE_STOP + ((PWM_MAX_FORWARD_SPEED-PWM_IDLE_STOP)*(percent/100.0));
	else
		speed = 0;

	// Set the left PWM to have an active time anywhere from 1-2ms.
	// A percentage of 0 should equate to an idle time of 1.5ms.
	//fprintf(stderr, "Setting left motor: %04x\n", speed);
	pwm_left(speed);
}

void right_motor(int percent) {
	int speed;
	percent *= -1;
	if(percent<-100)
		percent=-100;
	if(percent>100)
		percent=100;

	if(percent > -10 && percent < 10)
		percent = 0;

	if(percent)
		speed = PWM_IDLE_STOP + ((PWM_MAX_FORWARD_SPEED-PWM_IDLE_STOP)*(percent/100.0));
	else
		speed = 0;

	//fprintf(stderr, "Setting right motor: %04x\n", speed);
	pwm_right(speed);
}

