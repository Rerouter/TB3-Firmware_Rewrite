#include <Arduino.h>
#include "TB3_IO_ISR.h"
#include "TB3_Motor_Control.h"

extern const bool DEBUG_GOTO;
extern int inprogtype;

void goto_position(int gotoshot_temp);
