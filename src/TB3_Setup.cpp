#include "TB3_Setup.h"

void Setup_AUX_ON()
{
  int yUpDown = 0;

  // int displayvar=0;

  if (first_time == 1)
  {
    // prompt_val=AUX_ON;
    lcd.empty();
    draw(74, 1, 1); // lcd.at(1,1,"Aux Motor:");
    if (AUX_ON == 0)
      lcd.at(1, 12, "OFF");
    if (AUX_ON == 1)
      lcd.at(1, 12, "ON");
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();
  }

  AUX_ON = updateProgType(AUX_ON, joy_capture_y1(), 0, 1, 1);

  if (c_button || z_button)
  {

    eeprom_write(100, AUX_ON);
    // delay(350);
    // progstep_forward();

    if (c_button)
      progstep_forward();
    else
    {
      progtype = SETUPMENU;
      progstep_goto(0);
    }

    delay(350);
    UpdateNunChuck();
  }
}

void Setup_PAUSE_ENABLED()
{
  int yUpDown = 0;

  // int displayvar=0;

  if (first_time == 1)
  {
    lcd.empty();
    draw(62, 1, 1); // lcd.at(1,1,"Pause ")
    if (PAUSE_ENABLED == 0)
    {
      draw(68, 1, 8); // lcd.at(1,7,"Disabled")
    }
    if (PAUSE_ENABLED == 1)
    {
      draw(67, 1, 8); // lcd.at(1,7,"Enabled")
    }
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    // Serial.print("Read");Serial.println(NClastread);

    UpdateNunChuck();
  }

  PAUSE_ENABLED = updateProgType(PAUSE_ENABLED, joy_capture_y1(), 0, 1, 1);
  
  if (c_button || z_button)
  {

    eeprom_write(101, PAUSE_ENABLED);
    // progtype=0;
    if (c_button)
      progstep_forward();
    else
      progstep_backward();
    delay(350);
    UpdateNunChuck();
  }
}

void Setup_POWERSAVE_PT()
{
  int yUpDown = 0;

  // int displayvar=0;

  if (first_time == 1)
  {
    // prompt_val=POWERSAVE_PT;
    lcd.empty();
    lcd.at(1, 1, "PT Motors on");
    if (POWERSAVE_PT == 1)
      draw(70, 2, 1); // lcd.at(2,1,"Always");
    if (POWERSAVE_PT == 2)
      draw(71, 2, 1); // lcd.at(2,1,"Program");
    if (POWERSAVE_PT == 3)
      draw(72, 2, 1); // lcd.at(2,1,"Shoot (accuracy)");
    if (POWERSAVE_PT == 4)
      draw(73, 2, 1); // lcd.at(2,1,"Shoot (pwr save)");
    // lcd.at(2,12,"C-Set");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    // Serial.print("Read");Serial.println(NClastread);
    UpdateNunChuck();
  }

  POWERSAVE_PT = updateProgType(POWERSAVE_PT, joy_capture_y1(), 1, 4, 1);

  if (c_button || z_button)
  {

    // POWERSAVE_PT=prompt_val;
    eeprom_write(96, POWERSAVE_PT);
    progtype = 0;
    // delay(350);
    if (c_button)
      progstep_forward();
    else
      progstep_backward();
    delay(350);
    UpdateNunChuck();
  }
}

void Setup_POWERSAVE_AUX()
{
  int yUpDown = 0;

  // int displayvar=0;

  if (first_time == 1)
  {
    // prompt_val=POWERSAVE_AUX;
    lcd.empty();
    lcd.at(1, 1, "AUX Motors On:");
    if (POWERSAVE_AUX == 1)
      draw(70, 2, 1); // lcd.at(2,1,"Always");
    if (POWERSAVE_AUX == 2)
      draw(71, 2, 1); // lcd.at(2,1,"Program");
    if (POWERSAVE_AUX == 3)
      draw(72, 2, 1); // lcd.at(2,1,"Shoot (accuracy)");
    if (POWERSAVE_AUX == 4)
      draw(73, 2, 1); // lcd.at(2,1,"Shoot (pwr save)");
    // lcd.at(2,12,"C-Set");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    // Serial.print("Read");Serial.println(NClastread);
    UpdateNunChuck();
  }

  POWERSAVE_AUX = updateProgType(POWERSAVE_AUX, joy_capture_y1(), 1, 4, 1);

  if (c_button || z_button)
  {

    // POWERSAVE_AUX=prompt_val;
    eeprom_write(98, POWERSAVE_AUX);
    progtype = 0;
    // lcd.empty();
    if (c_button)
      progstep_forward();
    else
      progstep_backward();
    delay(350);
    UpdateNunChuck();
  }
}

void Setup_LCD_BRIGHTNESS_DURING_RUN()
{ // issue with this loop jumping out on first touch of up down - reads ghose C press.
  int yUpDown = 0;

  if (first_time == 1)
  {
    lcd.empty();
    lcd.at(1, 1, "BkLite On Run: ");
    lcd.at(1, 15, LCD_BRIGHTNESS_DURING_RUN);
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = 0;
    delay(250);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    // Serial.print("Read");Serial.println(NClastread);
    UpdateNunChuck();
  }

  LCD_BRIGHTNESS_DURING_RUN = updateProgType(LCD_BRIGHTNESS_DURING_RUN, joy_capture_y1(), 1, 8, 1);

  if (c_button || z_button)
  {

    // LCD Values
    eeprom_write(102, LCD_BRIGHTNESS_DURING_RUN);
    progtype = 0;
    // lcd.empty();
    // lcd.at(1,1,"Return Main Menu");
    lcd.bright(4);
    // delay(100);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    if (c_button)
      progstep_forward();
    else
      progstep_backward();
    delay(350);
    UpdateNunChuck();
  }
}

void Setup_Max_AUX_Motor_Speed()
{ // issue with this loop jumping out on first touch of up down - reads ghose C press.
  int yUpDown = 0;

  if (first_time == 1)
  {
    lcd.empty();
    lcd.at(1, 1, "Max Speed:  ");
    lcd.at(1, 12, AUX_MAX_JOG_STEPS_PER_SEC);
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = 0;
    delay(250);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    // Serial.print("Read");Serial.println(NClastread);
    UpdateNunChuck();
  }

  AUX_MAX_JOG_STEPS_PER_SEC = updateProgType(AUX_MAX_JOG_STEPS_PER_SEC, joy_capture_y1(), 2000, 20000, 500);

  if (c_button || z_button)
  {

    // POWERSAVE_AUX=prompt_val;
    eeprom_write(104, AUX_MAX_JOG_STEPS_PER_SEC);
    if (c_button)
      progstep_forward();
    else
      progstep_backward();
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last button
  }
}

void Setup_AUX_Motor_DIR()
{
  int yUpDown = 0;

  // int displayvar=0;

  if (first_time == 1)
  {
    // prompt_val=AUX_ON;
    lcd.empty();
    lcd.at(1, 1, "Aux Reversed:");
    if (AUX_REV == 0)
      lcd.at(1, 14, "OFF");
    if (AUX_REV == 1)
      lcd.at(1, 14, "ON");
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
    // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();
  }

  AUX_REV = updateProgType(AUX_REV, joy_capture_y1(), 0, 1, 1);

  if (c_button || z_button)
  {

    eeprom_write(106, AUX_REV);
    progtype = 0;
    lcd.empty();
    lcd.at(1, 1, "Return Main Menu");
    delay(1500);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last button
                           // progstep_forward();
    // progstep_goto(0);
    if (c_button)
      progstep_goto(0);
    else
      progstep_backward();
  }
}

void Set_Shot_Repeat()
{ //

  int yUpDown = 0;

  if (first_time == 1)
  {
    // int sequence_repeat_type=1; //1 Defaults - Run Once, 0 Continuous Loop,  -1 Repeat Forward
    lcd.empty();
    lcd.at(1, 1, "Select Shot Type");
    if (sequence_repeat_type == 1)
      lcd.at(2, 1, "Run Once");
    if (sequence_repeat_type == 0)
      lcd.at(2, 1, "Continuous Loop");
    if (sequence_repeat_type == -1)
      lcd.at(2, 1, "Repeat Forward"); // not currently supported
    // lcd.at(2,12,"C-Set");
    first_time = 0;
    delay(350);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
                           // delay(prompt_time);
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();
  }

  sequence_repeat_type = updateProgType(sequence_repeat_type, joy_capture_y1(), 0, 1, 1);

  if (c_button)
  {

    // POWERSAVE_AUX=prompt_val;
    // eeprom_write(98, POWERSAVE_AUX);
    // progtype=0;
    // lcd.empty();
    progstep_forward();
    delay(350);
    UpdateNunChuck();
  }
  else if (z_button)
  {

    // POWERSAVE_AUX=prompt_val;
    // eeprom_write(98, POWERSAVE_AUX);
    // progtype=0;
    // lcd.empty();
    progstep_backward();
    delay(350);
    UpdateNunChuck();
  }
}
