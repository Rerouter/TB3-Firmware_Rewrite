#ifndef TB3GOTO_H
#define TB3GOTO_H

#include <Arduino.h>
#include "TB3_IO_ISR.h"
#include "TB3_Motor_Control.h"

void goto_position(int gotoshot_temp);

#endif

extern const bool DEBUG_GOTO;
extern int inprogtype;