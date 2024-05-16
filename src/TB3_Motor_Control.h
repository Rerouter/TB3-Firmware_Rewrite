#ifndef TB3MOTOR_H
#define TB3MOTOR_H

#include <Arduino.h>
#include "TB3_IO_ISR.h"
#include "Structs.h"

void move_motors();
long motor_get_steps_2pt(int motor);
long motor_get_steps_3pt(int motor);
float catmullrom(float t, float p0, float p1, float p2, float p3);
long motor_get_steps_2pt_video(int motor);
void go_to_origin_max_speed();
void go_to_origin_slow();
void DisplayMove(int motorIndex);
void go_to_start_old();
void go_to_start_new();

#endif


extern unsigned int camera_fired;
extern unsigned int keyframe[2][6];
extern float linear_steps_per_shot[3];
extern float motor_steps_pt[3][3];
extern float ramp_params_steps[3];

extern const uint8_t MS1;
extern const uint8_t MS2;
extern const uint8_t MS3;

extern FloatPoint current_steps;
extern FloatPoint delta_steps;
extern Motor motors[];

extern unsigned int program_progress_2PT;
extern const bool DEBUG_MOTOR;
extern int8_t progtype;
extern const uint8_t REG2POINTMOVE;
extern const uint8_t REV2POINTMOVE;
extern const uint8_t REG3POINTMOVE;
extern const uint8_t REV3POINTMOVE;
extern const uint8_t AUXDISTANCE;
extern float percent;
extern unsigned int intval;
extern const uint8_t VIDEO_INTVAL;
extern unsigned int program_progress_3PT;
extern uint32_t feedrate_micros;
extern unsigned int lead_in;
extern unsigned int lead_out;
extern unsigned int rampval;
extern boolean Move_Engaged;
extern byte motorMoving;
extern boolean REVERSE_PROG_ORDER;
extern boolean MOVE_REVERSED_FOR_RUN;
extern const uint8_t MOTORS;
extern unsigned int camera_moving_shots;
extern const uint8_t MAX_MOVE_POINTS;
extern unsigned int prompt_time;
extern int sequence_repeat_type;
extern uint8_t POWERSAVE_PT;
extern uint8_t POWERSAVE_AUX;

extern long calculate_feedrate_delay_video();
extern void set_target(float x, float y, float z);
extern void dda_move(long micro_delay);
extern long calculate_feedrate_delay_1();
extern void updateMotorVelocities();
extern void synched3PtMove_max(float xtarget, float ytarget, float ztarget);
extern void set_position(float x, float y, float z);
extern void enable_PT();
extern void enable_AUX();
extern void disable_PT();
extern void disable_AUX();
extern void draw(int array_num, int col, int row);