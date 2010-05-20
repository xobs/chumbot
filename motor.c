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
	pwm_left(PWM_IDLE_MIN-((PWM_IDLE_MIN - PWM_MAX_FORWARD_SPEED) * percent / 100));
}

void right_motor(int percent) {
	pwm_right(PWM_IDLE_MAX-((PWM_IDLE_MAX - PWM_MAX_REVERSE_SPEED) * percent / 100));
}

/*
void stop() {
	pwm_drive(PWM_IDLE_MIN);
}

void turn_left(int percent) {
	pwm_turn(PWM_IDLE_MAX+((PWM_MAX_LEFT_TURN-PWM_IDLE_MAX) * percent / 100));
//	ltemp = PWM_IDLE_MAX+((PWM_MAX_LEFT_TURN-PWM_IDLE_MAX) * percent / 100);
//	fprintf(stderr,"TURN LEFT PWM VALUE: %x\n\n",ltemp);
}

void turn_right(int percent) {
	pwm_turn(PWM_IDLE_MIN-((PWM_IDLE_MIN - PWM_MAX_RIGHT_TURN) * percent / 100));
}

void turn_center() {
	pwm_turn(PWM_IDLE_MIN);
}

void init_car() {
	pwm_init(); // configure pwm pins
	pwm_drive(PWM_IDLE_MIN); // make sure wheels are not spinning
	pwm_turn(PWM_IDLE_MIN); // center the wheels
}
*/
