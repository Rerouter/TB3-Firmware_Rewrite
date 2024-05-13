#include <Arduino.h>
#include "NHDLCD9.h"
#include "Structs.h"

extern unsigned int first_time;
extern byte AUX_ON;
extern NHDLCD9 lcd;
extern unsigned int prompt_time;
extern volatile boolean nextMoveLoaded;
extern unsigned int joy_y_lock_count;
extern int prompt_delay;
extern int c_button;
extern int z_button;
extern const bool DEBUG_PANO;
extern long NClastread;
extern unsigned int camera_total_shots;
extern unsigned long diplay_last_tm;
extern int reviewprog;
extern float joy_y_axis;
extern unsigned int intval;
extern unsigned int static_tm;
extern unsigned long interval;
extern unsigned long interval_tm;
extern unsigned int progstep;
extern unsigned int camera_fired;
extern boolean Program_Engaged;
extern boolean Interrupt_Fire_Engaged;
extern unsigned int start_delay_sec;
extern unsigned int P2PType;
extern boolean Move_Engaged;
extern float motor_steps_pt[3][3];
extern FloatPoint current_steps;
extern unsigned int POWERSAVE_PT;
extern unsigned int POWERSAVE_AUX;
extern unsigned int progtype;

extern const uint16_t PAN_MAX_JOG_STEPS_PER_SEC;
extern const uint16_t TILT_MAX_JOG_STEPS_PER_SEC;
extern Motor motors[];
extern FloatPoint fp;

extern const uint8_t PANOGIGA;
extern const uint8_t PORTRAITPANO;

extern const uint8_t PANO_9ShotCenter;
extern const uint8_t PANO_25ShotCenter;
extern const uint8_t PANO_7X3;
extern const uint8_t PANO_9X5Type1;
extern const uint8_t PANO_9X5Type2;
extern const uint8_t PANO_5x5TopThird;
extern const uint8_t PANO_7X5TopThird;

extern void draw(int array_num, int col, int row);
extern void NunChuckQuerywithEC();
extern void NunChuckjoybuttons();
extern void DFSetup();
extern void axis_button_deadzone();
extern void updateMotorVelocities2();
extern void button_actions_move_x(int point);
extern int joy_capture_y1();
extern int joy_capture3();
extern void progstep_forward();
extern void progstep_backward();
extern void progstep_goto(unsigned int prgstp);
extern void disable_AUX();
extern void set_target(float x, float y, float z);
extern void dda_move(long micro_delay);
extern void setPulsesPerSecond(int motorIndex, uint16_t pulsesPerSecond);
extern void setupMotorMove(int motorIndex, int32_t destination);
extern void updateMotorVelocities();
extern void disable_PT();
extern void display_status();

void Set_angle_of_view();
void Define_Overlap_Percentage();
void Display_olpercentage();
void button_actions_olpercentage();
void Set_PanoArrayType();
String steps_to_deg_decimal(unsigned long steps);
void Pano_Review_Confirm();
void pano_button_actions_review();
void Pano_DisplayReviewProg();
void move_motors_pano_basic();
void move_motors_pano_accel();
void move_motors_accel_array();
void Move_to_Origin();
void calc_pano_move();
void button_actions290();
