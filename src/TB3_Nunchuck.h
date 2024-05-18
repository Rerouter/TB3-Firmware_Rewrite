#ifndef TB3NUNCHUCK_H
#define TB3NUNCHUCK_H

#include <Arduino.h>
#include "WiiNunchuck3.h"
#include "Structs.h"
#include "NHDLCD9.h"

void calibrate_joystick(int tempx, int tempy);
void NunChuckQuerywithEC();
void UpdateNunChuck();
void applyjoymovebuffer_exponential();
void applyjoymovebuffer_linear();
void nc_sleep();
void updateMotorVelocities2();

inline int joystick_deadband(int joy_axis, int threshold);
inline int JoystickLockCounter(int lockCount, int joyAxisValue, int threshold);

#endif

extern int joy_x_axis;
extern int joy_y_axis;
extern int accel_x_axis;
extern unsigned int joy_x_lock_count;
extern unsigned int joy_y_lock_count;
extern ButtonState c_button;
extern ButtonState z_button;

extern boolean AUX_REV;
extern boolean AUX_ON;
extern int prev_joy_x_reading;
extern int prev_joy_y_reading;
extern int prev_accel_x_reading;

extern uint16_t motorMoveSteps0;
extern uint16_t motorMoveSteps1;
extern uint16_t motorMoveSteps2;
extern volatile boolean nextMoveLoaded;
extern byte motorMoving;
extern uint32_t feedrate_micros;
extern const uint16_t PAN_MAX_JOG_STEPS_PER_SEC;
extern const uint16_t TILT_MAX_JOG_STEPS_PER_SEC;
extern uint16_t AUX_MAX_JOG_STEPS_PER_SEC;
extern const uint8_t MOTOR_EN;
extern const uint8_t MOTOR0_STEP;

extern FloatPoint fp;
extern FloatPoint current_steps;
extern Motor motors[];
extern uint16_t *motorAccumulator[];
extern NHDLCD9 lcd;

extern void set_target(float x, float y, float z);
extern long calculate_feedrate_delay_2();