#ifndef TB3SETUP_H
#define TB3SETUP_H

#include <Arduino.h>
#include "NHDLCD9.h"
#include "TB3_LCD_Buttons.h"

void Setup_AUX_ON();
void Setup_PAUSE_ENABLED();
void Setup_POWERSAVE_PT();
void Setup_POWERSAVE_AUX();
void Setup_LCD_BRIGHTNESS_DURING_RUN();
void Setup_Max_AUX_Motor_Speed();
void Setup_AUX_Motor_DIR();
void Set_Shot_Repeat();

#endif

extern long NClastread;
extern boolean c_button;
extern boolean z_button;
extern const uint8_t SETUPMENU;
extern NHDLCD9 lcd;

extern void draw(int array_num, int col, int row);
extern void UpdateNunChuck();
extern int joy_capture_y1();
extern void progstep_forward();
extern void progstep_backward();
extern void progstep_goto(unsigned int prgstp);