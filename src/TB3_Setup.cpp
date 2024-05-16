#include "TB3_Setup.h"

void Setup_AUX_ON()
{
  if (first_time)
  {
    lcd.empty();
    draw(74, 1, 1); // lcd.at(1,1,"Aux Motor:");
    if (AUX_ON)
      lcd.at(1, 12, "ON");
    else
      lcd.at(1, 12, "OFF");

    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastAUX_ON = AUX_ON;
    AUX_ON = updateProgType(AUX_ON, joy_capture_y1(), 0, 1, 1);
    if (lastAUX_ON != AUX_ON) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(100, AUX_ON);

      if (c_button)
        progstep_forward();
      else
      {
        progtype = SETUPMENU;
        progstep_goto(0);
      }

      delay(prompt_time); // Exit Delay
      UpdateNunChuck(); // Clear the last button action
    }
  }
}

void Setup_PAUSE_ENABLED()
{
  if (first_time)
  {
    lcd.empty();
    draw(62, 1, 1); // lcd.at(1,1,"Pause ")
    if (PAUSE_ENABLED)
    {
      draw(67, 1, 8); // lcd.at(1,7,"Enabled")
    }
    else
    {
      draw(68, 1, 8); // lcd.at(1,7,"Disabled")
    }
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastPAUSE_ENABLED = PAUSE_ENABLED;
    PAUSE_ENABLED = updateProgType(PAUSE_ENABLED, joy_capture_y1(), 0, 1, 1);
    if (lastPAUSE_ENABLED != PAUSE_ENABLED) {first_time = true;}
  
    if (c_button || z_button)
    {
      eeprom_write(101, PAUSE_ENABLED);
      // progtype=0;
      if (c_button)
        progstep_forward();
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck();
    }
  }
}

void Setup_POWERSAVE_PT()
{
  if (first_time)
  {
    lcd.empty();
    lcd.at(1, 1, "PT Motors on");
    switch(POWERSAVE_PT) {
      case 1:
        draw(70, 2, 1); // lcd.at(2,1,"Always");
        break;
      case 2:
        draw(71, 2, 1); // lcd.at(2,1,"Program");
        break;
      case 3:
        draw(72, 2, 1); // lcd.at(2,1,"Shoot (accuracy)");
        break;
      case 4:
        draw(73, 2, 1); // lcd.at(2,1,"Shoot (pwr save)");
        break;
    }      
      
    // lcd.at(2,12,"C-Set");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastPOWERSAVE_PT = POWERSAVE_PT;
    POWERSAVE_PT = updateProgType(POWERSAVE_PT, joy_capture_y1(), 1, 4, 1);
    if (lastPOWERSAVE_PT != POWERSAVE_PT) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(96, POWERSAVE_PT);
      progtype = 0;
      if (c_button)
        progstep_forward();
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck();
    }
  }  
}

void Setup_POWERSAVE_AUX()
{
  if (first_time)
  {
    lcd.empty();
    lcd.at(1, 1, "AUX Motors On:");

    switch (POWERSAVE_AUX)
    {
    case 1:
      draw(70, 2, 1); // lcd.at(2,1,"Always");
      break;
    case 2:
      draw(71, 2, 1); // lcd.at(2,1,"Program");
      break;
    case 3:
      draw(72, 2, 1); // lcd.at(2,1,"Shoot (accuracy)");
      break;
    case 4:
      draw(73, 2, 1); // lcd.at(2,1,"Shoot (pwr save)");
      break;
    }

    // lcd.at(2,12,"C-Set");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastPOWERSAVE_AUX = POWERSAVE_AUX;
    POWERSAVE_AUX = updateProgType(POWERSAVE_AUX, joy_capture_y1(), 1, 4, 1);
    if (lastPOWERSAVE_AUX != POWERSAVE_AUX) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(98, POWERSAVE_AUX);
      progtype = 0;
      // lcd.empty();
      if (c_button)
        progstep_forward();
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck();
    }
  }
}

void Setup_LCD_BRIGHTNESS_DURING_RUN()
{ // issue with this loop jumping out on first touch of up down - reads ghose C press.
  if (first_time)
  {
    lcd.empty();
    lcd.bright(LCD_BRIGHTNESS_DURING_RUN);
    lcd.at(1, 1, "BkLite On Run: ");
    lcd.at(1, 15, LCD_BRIGHTNESS_DURING_RUN);
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastLCD_BRIGHTNESS_DURING_RUN = LCD_BRIGHTNESS_DURING_RUN;
    LCD_BRIGHTNESS_DURING_RUN = updateProgType(LCD_BRIGHTNESS_DURING_RUN, joy_capture_y1(), 1, 8, 1);
    if (lastLCD_BRIGHTNESS_DURING_RUN != LCD_BRIGHTNESS_DURING_RUN) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(102, LCD_BRIGHTNESS_DURING_RUN);
      progtype = 0;
      // lcd.empty();
      // lcd.at(1,1,"Return Main Menu");
      lcd.bright(4);
      if (c_button)
        progstep_forward();
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck();
    }
  }
}

void Setup_Max_AUX_Motor_Speed()
{ // issue with this loop jumping out on first touch of up down - reads ghose C press.

  if (first_time)
  {
    lcd.empty();
    lcd.at(1, 1, "Max Speed:  ");
    lcd.at(1, 12, AUX_MAX_JOG_STEPS_PER_SEC);
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastAUX_MAX_JOG_STEPS_PER_SEC = AUX_MAX_JOG_STEPS_PER_SEC;
    AUX_MAX_JOG_STEPS_PER_SEC = updateProgType(AUX_MAX_JOG_STEPS_PER_SEC, joy_capture_y1(), 2000, 20000, 500);
    if (lastAUX_MAX_JOG_STEPS_PER_SEC != AUX_MAX_JOG_STEPS_PER_SEC) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(104, AUX_MAX_JOG_STEPS_PER_SEC);
      if (c_button)
        progstep_forward();
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck(); //  Use this to clear out any button registry from the last button
    }
  }
}

void Setup_AUX_Motor_DIR()
{
  if (first_time)
  {
    lcd.empty();
    lcd.at(1, 1, "Aux Reversed:");
    if (AUX_REV)
      lcd.at(1, 14, "ON");
    else
      lcd.at(1, 14, "OFF");
      
    draw(65, 2, 1); // lcd.at(2,1,"UpDown  C-Select");
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();

    auto lastAUX_REV = AUX_REV;
    AUX_REV = updateProgType(AUX_REV, joy_capture_y1(), 0, 1, 1);
    if (lastAUX_REV != AUX_REV) {first_time = true;}

    if (c_button || z_button)
    {
      eeprom_write(106, AUX_REV);
      progtype = 0;
      lcd.empty();
      lcd.at(1, 1, "Return Main Menu");
      if (c_button)
        progstep_goto(0);
      else
        progstep_backward();
      delay(prompt_time);
      UpdateNunChuck(); //  Use this to clear out any button registry from the last button
                            // progstep_forward();
    }
  }
}

void Set_Shot_Repeat()
{
  if (first_time)
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
    first_time = false;
    delay(prompt_time);
    UpdateNunChuck(); //  Use this to clear out any button registry from the last step
  }

  if ((millis() - NClastread) > 50)
  {
    NClastread = millis();
    UpdateNunChuck();
    
    auto lastsequence_repeat_type = sequence_repeat_type;
    sequence_repeat_type = updateProgType(sequence_repeat_type, joy_capture_y1(), 0, 1, 1);
    if (lastsequence_repeat_type != sequence_repeat_type) {first_time = true;}

    if (c_button)
    {
      // eeprom_write(98, POWERSAVE_AUX);
      // progtype=0;
      // lcd.empty();
      progstep_forward();
    }
    else if (z_button)
    {
      // eeprom_write(98, POWERSAVE_AUX);
      // progtype=0;
      // lcd.empty();
      progstep_backward();
    }
    delay(prompt_time);
    UpdateNunChuck();
  }
}
