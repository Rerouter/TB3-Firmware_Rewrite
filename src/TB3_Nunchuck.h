#include <Arduino.h>
#include "WiiNunchuck3.h"
#include "Structs.h"

extern float joy_x_axis;
extern float joy_y_axis;
extern float accel_x_axis;
extern byte AUX_REV;
extern byte AUX_ON;
extern unsigned int joy_x_lock_count;
extern unsigned int joy_y_lock_count;
extern int c_button;
extern int z_button;
extern boolean C_Released;
extern boolean Z_Released;
extern boolean CZ_Released;
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

extern FloatPoint fp;
extern FloatPoint current_steps;
extern Motor motors[];
extern uint16_t *motorAccumulator[];

extern void set_target(float x, float y, float z);
extern long calculate_feedrate_delay_2();

void calibrate_joystick(int tempx, int tempy);
void NunChuckQuerywithEC();
void NunChuckjoybuttons();
void applyjoymovebuffer_exponential();
void applyjoymovebuffer_linear();
void nc_sleep();
void axis_button_deadzone();
void updateMotorVelocities2();
