#include "TB3_InShootMenu.h"

int Z_Button_Read_Count = 0;
int C_Button_Read_Count = 0;
unsigned int goto_shot = 0;
uint32_t input_last_tm = 0;

// In Program Menu Ordering
const uint8_t INPROG_OPTIONS = 5; // up this when code for gotoframe

const uint8_t INPROG_RESUME = 0;
const uint8_t INPROG_RTS = 1;         // return to start
const uint8_t INPROG_GOTO_END = 2;    // Go to end
const uint8_t INPROG_GOTO_FRAME = 3;  // go to frame
const uint8_t INPROG_INTERVAL = 4;    // Set Interval
const uint8_t INPROG_STOPMOTION = 99; // Manual Forward and Back

void Check_Prog() // this is a routine for the button presses in the program
{
  if (c_button && z_button)
  {
    C_Button_Read_Count++;
    Z_Button_Read_Count++;
    CZ_Button_Read_Count++;

    delay(10);
    if ((millis() - input_last_tm) > 2000)
    {
      C_Button_Read_Count = 0;
      Z_Button_Read_Count = 0;
      CZ_Button_Read_Count = 0;
      input_last_tm = millis();
    }
  }

  if (c_button && !z_button)
  {
    C_Button_Read_Count++; // c button on
    if ((millis() - input_last_tm) > 2000)
    {
      C_Button_Read_Count = 0;
      input_last_tm = millis();
    }
  }

  if (z_button && !c_button)  // this would send us back to step 5
  {
    Z_Button_Read_Count++; // z button only on
    if ((millis() - input_last_tm) > 2000)
    {
      Z_Button_Read_Count = 0;
      input_last_tm = millis();
    }
  }
}

/*
void Program_Engaged_Toggle()    {  //used for pausing
      CZ_Button_Read_Count=0;
      CZ_Released=false; //to prevent entry into this method until CZ button release again
      Program_Engaged=!Program_Engaged; //toggle off the loop

}
*/

void SMS_In_Shoot_Paused_Menu() // this runs once and is quick - not persistent
{
  CZ_Button_Read_Count = 0;
  CZ_Released = false;     // to prevent entry into this method until CZ button release again
  Program_Engaged = false; // toggle off the loop
  if (POWERSAVE_PT > 2)
    disable_PT();
  if (POWERSAVE_AUX > 2)
    disable_AUX();
  inprogtype = 0;      // default this to the first option, Resume
  progstep_goto(1001); // send us to a loop where we can select options
}

void SMS_Resume() // this runs once and is quick - not persistent
{
  lcd.empty();
  lcd.at(1, 1, "Resuming");
  CZ_Button_Read_Count = 0;
  CZ_Released = false;    // to prevent entry into this method until CZ button release again
  Program_Engaged = true; // toggle off the loop
  delay(prompt_time);
  progstep_goto(50); // send us back to the main SMS Loop
}

void InProg_Select_Option()
{
  // int xLeftRight=0;

  if (first_time)
  {
    lcd.empty();
    // Assuming inprogtype is an integer or enum type
    switch (inprogtype) {
        case INPROG_RESUME: // 0
            draw(86, 1, 6);  // lcd.at(1,6,"Resume");
            break;
        case INPROG_RTS: // 1
            draw(87, 1, 6);  // lcd.at(1,6,"Restart");
            break;
        case INPROG_GOTO_END: // 2
            draw(89, 1, 5);  // lcd.at(1,5,"Go to End");
            break;
        case INPROG_GOTO_FRAME: // 3
            draw(88, 1, 1);  // lcd.at(1,1,"GoTo Frame:");
            goto_shot = camera_fired;
            DisplayGoToShot();
            break;
        case INPROG_INTERVAL: // 4
            // If there are issues with draw command, it may be safer to use lcd.at directly as done below
            intval = interval / 100;
            lcd.at(1, 1, "Intval:   .  sec");
            DisplayInterval();
            break;
        case INPROG_STOPMOTION: // 99?
            lcd.at(1, 1, "StopMo R+C / L+C");
            break;
        default:
            lcd.at(1, 1, "Undefined Option");
            break;
    }
    lcd.at(2, 1, "UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    NClastread = millis();
    if (POWERSAVE_PT > 2)
      disable_PT();
    if (POWERSAVE_AUX > 2)
      disable_AUX();
  } // end first time

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    if (inprogtype == INPROG_GOTO_FRAME)
    { // read leftright values for the goto frames
      unsigned int goto_shot_last = goto_shot;
      goto_shot = updateProgType(goto_shot, joy_capture_x(), 1, camera_total_shots, 1);
      if (goto_shot_last != goto_shot) { DisplayGoToShot(); }
    }

    if (inprogtype == INPROG_INTERVAL)
    { // read leftright values for the goto frames
      unsigned int intval_last = intval;
      intval = updateProgType(intval, joy_capture_x(), 5, 6000, 1);
      if (intval_last != intval) { DisplayInterval(); }
    }

    inprogtype = updateProgType(inprogtype, joy_capture_y1(), 0, INPROG_OPTIONS - 1, 1);
    if (CZ_Released)
      button_actions_InProg_Select_Option(); // don't react to buttons unless there has been a CZ release
    delay(prompt_time);
    }
}

void button_actions_InProg_Select_Option()
{
  if (c_button && !z_button)
  {
    lcd.empty();
    if (inprogtype == INPROG_RESUME)
    { // Resume (unpause)
      SMS_Resume();
    }
    else if (inprogtype == INPROG_RTS)
    { // Return to restart the shot  - send to review screen of relative move
      REVERSE_PROG_ORDER = false;
      // if (POWERSAVE_PT>2)   disable_PT();
      // if (POWERSAVE_AUX>2)   disable_AUX();
      // Program_Engaged=true;
      camera_fired = 0;
      lcd.bright(8);
      lcd.at(1, 2, "Going to Start");

      if (progtype == REG2POINTMOVE || progtype == REV2POINTMOVE)
      {
        go_to_start_new();
        progstep_goto(8);
      }
      else if (progtype == REG3POINTMOVE || progtype == REV3POINTMOVE)
      {
        go_to_start_new();
        progstep_goto(109);
      }
      else if (progtype == AUXDISTANCE)
      {
        go_to_start_new();
        progstep_goto(8);
      }
    }
    else if (inprogtype == INPROG_GOTO_END)
    { // Go to end point - basically a reverse move setup from wherever we are.
      REVERSE_PROG_ORDER = true;
      // if (POWERSAVE_PT>2)   disable_PT();
      // if (POWERSAVE_AUX>2)   disable_AUX();
      // Program_Engaged=true;
      camera_fired = 0;
      lcd.bright(8);
      lcd.at(1, 3, "Going to End");

      if (progtype == REG2POINTMOVE || progtype == REV2POINTMOVE)
      {
        go_to_start_new();
        progstep_goto(8);
      }
      else if (progtype == REG3POINTMOVE || progtype == REV3POINTMOVE)
      {
        go_to_start_new();
        progstep_goto(109);
      }
      else if (progtype == AUXDISTANCE)
      {
        go_to_start_new();
        progstep_goto(8);
      }
    }

    else if (inprogtype == INPROG_GOTO_FRAME)
    { // Go to specific frame
      first_time = true;
      lcd.at(1, 4, "Going to");
      lcd.at(2, 4, "Frame:");
      lcd.at(2, 11, goto_shot);
      goto_position(goto_shot);
      inprogtype = INPROG_RESUME;
    }

    else if (inprogtype == INPROG_INTERVAL)
    { // Change Interval and static time
      first_time = true;
      // look at current gap between interval and static time = available move time.
      unsigned int available_move_time = interval / 100 - static_tm; // this is the gap we keep interval isn't live
      // Serial.print("AMT:");Serial.println(available_move_time);
      if (available_move_time <= MIN_INTERVAL_STATIC_GAP)
        available_move_time = MIN_INTERVAL_STATIC_GAP; // enforce min gap between static and interval
      interval = (long)intval * 100;                   // set the new ms timer for SMS
      if (intval > available_move_time)
      { // we can apply the gap
        // Serial.print("intval-available_move_time pos: ");Serial.println(intval-available_move_time);
        static_tm = intval - available_move_time;
        // Serial.print("static_tm= ");Serial.println(static_tm);
      }
      else // squished it too much, go with minimum static time
      {
        static_tm = 1;
      }
      inprogtype = INPROG_RESUME;
    }
  }

  if (z_button && !c_button)
  {
    delay(1);
  }
}

void DisplayGoToShot()
{
  lcd.at(1, 13, goto_shot);
  if (goto_shot < 10)
    lcd.at(1, 14, "   "); // clear extra if goes from 3 to 2 or 2 to  1
  if (goto_shot < 100)
    lcd.at(1, 15, "  "); // clear extra if goes from 3 to 2 or 2 to  1
  if (goto_shot < 1000)
    lcd.at(1, 16, " "); // clear extra if goes from 3 to 2 or 2 to  1
}
