#ifndef TB3STEPPER_H
#define TB3STEPPER_H

#include <Arduino.h>
#include "Structs.h"

void init_steppers();
void dda_move(long micro_delay);
bool can_step(long current, long target);
void set_target(float x, float y, float z);
void set_position(float x, float y, float z);
void calculate_deltas();
long calculate_feedrate_delay_1();
long calculate_feedrate_delay_video();
long calculate_feedrate_delay_2();
void disable_PT();
void disable_AUX();
void enable_PT();
void enable_AUX();

#endif

extern boolean AUX_ON;
extern FloatPoint delta_steps;
extern FloatPoint current_steps;
extern FloatPoint target_steps;
extern Motor motors[];
extern const uint8_t MOTOR0_DIR;
extern const uint8_t MOTOR1_DIR;
extern const uint8_t MOTOR2_DIR;
extern const bool DEBUG_MOTOR;
extern unsigned int intval;
extern const uint8_t VIDEO_INTVAL;
extern unsigned long interval;
extern const uint8_t EXTTRIG_INTVAL;
extern unsigned int static_tm;
extern unsigned int prefire_time;
extern unsigned int program_progress_2PT;
extern unsigned int keyframe[2][6];
extern const uint32_t VIDEO_FEEDRATE_NUMERATOR;
extern const uint8_t MOTOR_EN;
extern const uint8_t MOTOR_EN2;