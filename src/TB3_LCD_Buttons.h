#include <Arduino.h>
#include "TB3_Camera_Control.h"
#include "TB3_IO_ISR.h"
#include "TB3_Motor_Control.h"
#include "TB3_EEPROM.h"
#include "TB3_PANO.h"

extern unsigned long Tilt_AOV_steps;
extern const uint8_t PINOUT_VERSION;
extern const uint16_t MAX_AUX_MOVE_DISTANCE;
extern float joy_x_axis;
extern float accel_x_axis;
extern unsigned long Pan_AOV_steps;
extern const uint8_t EXTTRIG_INTVAL;
extern const uint8_t MIN_INTERVAL_STATIC_GAP;
extern unsigned long start_delay_tm;
extern int ext_shutter_count;
extern boolean progstep_forward_dir;
extern int CZ_Button_Read_Count;
extern unsigned int joy_x_lock_count;
//extern char lcdbuffer1[];
extern uint16_t STEPS_PER_INCH_AUX;
extern const uint8_t SETUPMENU;

extern void DFloop();
extern void Check_Prog();

void Choose_Program();
void button_actions_choose_program();
void Move_to_Startpoint();
void button_actions_move_start();
void Move_to_Endpoint();
void button_actions_move_end();
void Move_to_Point_X(int Point);
void button_actions_move_x(int Point);
void Set_Cam_Interval();
void DisplayInterval();
void button_actions_intval(unsigned int intval);
void Set_Duration();
void Display_Duration();
void button_actions_overaldur();
void Set_Static_Time();
void DisplayStatic_tm();
void button_actions_stat_time(unsigned int exposure);
void Set_Ramp();
void DisplayRampval();
void button_actions_rampval();
void Set_LeadIn_LeadOut();
void DisplayLeadIn_LeadOut();
void button_actions_lead_in_out();
void Calculate_Shot();
void Review_Confirm();
void DisplayReviewProg();
void button_actions_review();
void progstep_forward();
void progstep_backward();
void progstep_goto(unsigned int prgstp);
void button_actions_end_of_program();
void Auto_Repeat_Video();
int joy_capture2();
int joy_capture3();
int joy_capture_4();
int joy_capture_x1();
int joy_capture_y1();
int joy_capture_x3();
void display_status();
void calc_time_remain();
void calc_time_remain_dur_sec(unsigned int sec);
void calc_time_remain_start_delay();
void display_time(int row, int col);
void draw(int array_num, int col, int row);
void Enter_Aux_Endpoint();
void DisplayAUX_Dist();
void button_actions_Enter_Aux_Endpoint();
