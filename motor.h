#ifndef __MOTOR_H__
#define __MOTOR_H__
/*
#define PWM_MAX_RIGHT_TURN      0x0B9A
#define PWM_MAX_FORWARD_SPEED   0x11F0
//#define PWM_CENTER          0x119B
//
#define PWM_CENTER_MIN          0x11DB
#define PWM_CENTER              0x11BB
#define PWM_CENTER_MAX          0x11CB

#define PWM_IDLE_MIN            0x1288
#define PWM_IDLE_STOP           0x12A4
#define PWM_IDLE_MAX            0x12C0

//#define PWM_MAX_REVERSE_SPEED   0x1324
#define PWM_MAX_LEFT_TURN       0x18BA
*/

#define PWM_MAX_FORWARD_SPEED   0x11F0
#define PWM_MAX_REVERSE_SPEED   0x1350

int pwm_init();
int pwm_turn(int value);
int pwm_drive(int value);
#endif // __MOTOR_H__
