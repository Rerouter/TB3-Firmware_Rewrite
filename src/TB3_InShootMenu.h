#include <Arduino.h>
#include "TB3_Motor_Control.h"
#include "TB3_LCD_Buttons.h"

extern int CZ_Button_Read_Count;
extern boolean CZ_Released;
extern int inprogtype;

extern void goto_position(int gotoshot_temp);

void Check_Prog();
void SMS_In_Shoot_Paused_Menu();
void SMS_Resume();
void InProg_Select_Option();
void button_actions_InProg_Select_Option();
void DisplayGoToShot();
