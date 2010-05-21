#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "motor.h"
#include "cylon.h"
#include "SDL.h"
#include "InputEvent.h"
#include "USBMissileLauncher.h"


#define USB_TIMEOUT 1000 /* milliseconds */
int debug_level = 0;

int open_joystick(char *filename) {
	int fd;
	while((fd = open(filename, O_RDONLY)) < 0)
		sleep(1);
	return fd;
}

int main(int argc, char **argv) {
	SDL_Joystick *joystick;
	missile_usb *missile_control;
	int done = 0;
	int missile_type = 1;
	int hat_value = 0;

	// Initialize motors.
	pwm_init();


	// Initialize cylon lights.
	cylon_init();
	cylon_start();


	// Initialize joystick.
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	fprintf(stderr, "%d joysticks detected\n", SDL_NumJoysticks());
	if(!(joystick = SDL_JoystickOpen(0))) {
		fprintf(stderr, "Couldn't open joystick: %s\n", SDL_GetError());
		return 2;
	}


	// Initialize USB Missile launcher.
	if(missile_usb_initialise() != 0) {
		fprintf(stderr, "missile_usb_initalise failed: %s\n", strerror(errno));
		return -1;
	}

	missile_control = missile_usb_create(debug_level, USB_TIMEOUT);
	if(missile_control == NULL) {
		fprintf(stderr, "missile_usb_create() failed\n");
		return -1;
	}

	if(missile_usb_finddevice(missile_control, 0, missile_type) != 0) {
		fprintf(stderr, "warning: USBMissileLauncher device not found\n");
		missile_control = NULL;
	}


	while(!done) {
		SDL_Event e;
		int new_hat_value = hat_value;

		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_JOYAXISMOTION: {
					int val = e.jaxis.value;
					val *= 100;
					val /= 32768;
					// Axes:
					//   0 == left stick left/right
					//   1 == left stick up/down
					//   2 == left trigger
					//   3 == right stick left/right
					//   4 == right stick up/down
					//   5 == right trigger
					if(e.jaxis.axis == 1) {
						left_motor(val);
					}
					else if(e.jaxis.axis == 4) {
						right_motor(val);
					}
					break;
				}

				case SDL_JOYHATMOTION:
					new_hat_value = e.jhat.value;
					break;

				case SDL_JOYBUTTONDOWN:
					if(e.jbutton.button == 0)
						new_hat_value |= 0x1000;
					break;

				case SDL_JOYBUTTONUP:
					if(e.jbutton.button == 0)
						hat_value &= (~0x1000);
					break;

				case SDL_NOEVENT:
					cylon_stop();
					break;

				case SDL_QUIT:
					done = 1;
					break;

				default:
					fprintf(stderr, "Unhandled event %d\n", e.type);
					break;
			}
		}

		// If the hat value has changed, send a new missile packet.
		if(missile_control && (new_hat_value != hat_value)) {
			char missile_message = 0;
			hat_value = new_hat_value;

			if(hat_value & SDL_HAT_DOWN)
				missile_message |= MISSILE_LAUNCHER_CMD_DOWN;

			if(hat_value & SDL_HAT_UP)
				missile_message |= MISSILE_LAUNCHER_CMD_UP;

			if(hat_value & SDL_HAT_LEFT)
				missile_message |= MISSILE_LAUNCHER_CMD_LEFT;

			if(hat_value & SDL_HAT_RIGHT)
				missile_message |= MISSILE_LAUNCHER_CMD_RIGHT;

			if(hat_value & 0x1000)
				missile_message |= MISSILE_LAUNCHER_CMD_FIRE;

			missile_do(missile_control, missile_message, missile_type);
		}
	}
	return 0;
}
