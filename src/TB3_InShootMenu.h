#ifndef TB3SHOOT_H
#define TB3SHOOT_H

#include <Arduino.h>
#include "TB3_Motor_Control.h"
#include "TB3_LCD_Buttons.h"

void Check_Prog();
void SMS_In_Shoot_Paused_Menu();
void SMS_Resume();
void InProg_Select_Option();
void button_actions_InProg_Select_Option();
void DisplayGoToShot();

#endif

extern int CZ_Button_Read_Count;
extern boolean CZ_Released;
extern int inprogtype;

extern void goto_position(int gotoshot_temp);