#include "TB3_Nunchuck.h"

const bool DEBUG_NC = 0;

int joy_x_axis_Offset;
int joy_x_axis_Threshold;
int joy_y_axis_Offset;
int joy_y_axis_Threshold;
int accel_x_axis_Offset = 512;
int accel_x_axis_Threshold = 200;

const uint8_t joy_deadband = 15;
const uint8_t acc_deadband = 100;
const uint8_t joy_lock_theshold = 83;

uint32_t lastZChangeTime = 0;
uint32_t lastCChangeTime = 0;
const uint32_t debounceDelay = 50;  // Debounce time in milliseconds
const uint32_t holdThreshold = 1000;  // Time in milliseconds to consider as held


ButtonState UpdateCButton(boolean input, ButtonState state) {
  uint32_t currentTime = millis();

  // Check if state should change based on input and debounce
  if (currentTime - lastCChangeTime > debounceDelay) {
    switch (state) {
        case ButtonState::ReadAgain:
          if (!input) {
              state = ButtonState::Released;
              lastCChangeTime = currentTime;  // Reset timer at state change
          }
          break;
        case ButtonState::Released:
          if (input) {
              state = ButtonState::Pressed;
              lastCChangeTime = currentTime;  // Reset timer at state change
          }
          break;
        case ButtonState::Pressed:
        case ButtonState::Held:
          if (!input) {
              state = ButtonState::Released;
              lastCChangeTime = currentTime;  // Reset timer at state change
          } else if (currentTime - lastCChangeTime > holdThreshold) {
              state = ButtonState::Held;  // Transition to Held state
          }
          break;
    }
  }
  return state;
}

ButtonState UpdateZButton(boolean input, ButtonState state) {
  uint32_t currentTime = millis();

  // Check if state should change based on input and debounce
  if (currentTime - lastZChangeTime > debounceDelay) {
    switch (state) {
      case ButtonState::ReadAgain:
        if (!input) {
            state = ButtonState::Released;
            lastCChangeTime = currentTime;  // Reset timer at state change
        }
        break;
      case ButtonState::Released:
        if (input) {
            state = ButtonState::Pressed;
            lastZChangeTime = currentTime;  // Reset timer at state change
        }
        break;
      case ButtonState::Pressed:
      case ButtonState::Held:
        if (!input) {
            state = ButtonState::Released;
            lastZChangeTime = currentTime;  // Reset timer at state change
        } else if (currentTime - lastCChangeTime > holdThreshold) {
            state = ButtonState::Held;  // Transition to Held state
        }
        break;
    }
  }
  return state;
}

void calibrate_joystick(int tempx, int tempy)
{
  if (DEBUG_NC) {Serial.println(micros());}

  joy_x_axis_Offset = tempx;
  joy_y_axis_Offset = tempy;
  joy_x_axis_Threshold = 127 - abs(127 - joy_x_axis_Offset);
  joy_y_axis_Threshold = 127 - abs(127 - joy_y_axis_Offset);
}

void NunChuckQuerywithEC() // error correction and reinit on disconnect  - takes about 1050 microsecond
{
  static int NCReadStatus = 0;           // control variable for NC error handling
  
  if (DEBUG_NC) {Serial.println(micros());}

  do
  {
    Nunchuck.getData();

    // Store results in local variables to avoid multiple function calls
    uint8_t joyX = static_cast<uint8_t>(Nunchuck.joyx());
    uint8_t joyY = static_cast<uint8_t>(Nunchuck.joyy());
    uint16_t accelX = static_cast<uint16_t>(Nunchuck.accelx());  // Assuming 10-bit data
    uint16_t accelY = static_cast<uint16_t>(Nunchuck.accely());
    uint16_t accelZ = static_cast<uint16_t>(Nunchuck.accelz());

    if (joyX == 0 && joyY == 0)
    {
        // Error condition: throw this out and read again
        delay(1);
        NCReadStatus++;
    }
    else if (joyX == 255 && joyY == 255 && (accelX == 1023 || accelX == 0))
    {
        // Nunchuck disconnected, then reconnected - needs initializing
        Nunchuck.init(0);
        NCReadStatus++;
    }
    else if (accelX == 0 && accelY == 0 && accelZ == 0)
    {
        // Nunchuck just reinitialized - needs a few more reads before good
        delay(1);
        NCReadStatus++;
    }
    else
      NCReadStatus = 0;

    if (NCReadStatus > 250) {
      NCReadStatus = 250;
    }

  } while (NCReadStatus > 0);
  if (DEBUG_NC) {
    Serial.println(micros());
    Nunchuck.printData();
  }
}

inline int joystick_deadband(int joy_axis, int threshold) {
  // If in the deadband, return 0, otherwise subtract the deadband from the value
    return (abs(joy_axis) >= threshold) ? joy_axis + (joy_axis >= threshold ? -threshold : threshold) : 0;
}

inline int JoystickLockCounter(int lockCount, int joyAxisValue) {
    return (abs(joyAxisValue) > joy_lock_theshold) ? ((lockCount + 1 > 250) ? 250 : lockCount + 1) : 0;
}

void UpdateNunChuck()
{
  NunChuckQuerywithEC();
  joy_x_axis = constrain((Nunchuck.joyx() - joy_x_axis_Offset), -joy_x_axis_Threshold, joy_x_axis_Threshold);
  joy_y_axis = constrain((Nunchuck.joyy() - joy_y_axis_Offset), -joy_y_axis_Threshold, joy_y_axis_Threshold);
  accel_x_axis = constrain((Nunchuck.accelx() - accel_x_axis_Offset), -accel_x_axis_Threshold, accel_x_axis_Threshold);
  if (AUX_REV)
    accel_x_axis *= -1;

  //create a deadband
  joy_x_axis   = joystick_deadband(joy_x_axis, joy_deadband);
  joy_y_axis   = joystick_deadband(joy_y_axis, joy_deadband);
  accel_x_axis   = joystick_deadband(accel_x_axis, acc_deadband);

  joy_x_lock_count = JoystickLockCounter(joy_x_lock_count, joy_x_axis);
  joy_y_lock_count = JoystickLockCounter(joy_y_lock_count, joy_y_axis);

  c_button = UpdateCButton(Nunchuck.cbutton(), c_button);
  z_button = UpdateZButton(Nunchuck.zbutton(), z_button);
}

void applyjoymovebuffer_exponential() { // exponential stuff
  // scale based on read frequency  base is 500 reads per second  - now 20 reads per second = 25x
  float joy_x = (long(joy_x_axis) * joy_x_axis * joy_x_axis) / 1200L;
  float joy_y = (long(joy_y_axis) * joy_y_axis * joy_y_axis) / 1200L;
  float accel_x = (long(accel_x_axis) * accel_x_axis * accel_x_axis) / 300L;
  // joy_x_axis=(joy_x_axis*joy_x_axis)/4;
  // joy_y_axis*=25;
  // accel_x_axis*=25;

  // control max speeds of the axis
  // joy_y_axis=map(joy_y_axis,-90,90,-35,35); //reduc

  // slow down changes to avoid sudden stops and starts
  // int ss_buffer=100;
  int buffer_x;
  int buffer_y;
  int buffer_z;

  // if ((joy_x_axis-prev_joy_x_reading)>ss_buffer) joy_x_axis=(prev_joy_x_reading+ss_buffer);
  // else if ((joy_x_axis-prev_joy_x_reading)<-ss_buffer) joy_x_axis=(prev_joy_x_reading-ss_buffer);

  buffer_x = (joy_x - prev_joy_x_reading) / 5;
  joy_x = prev_joy_x_reading + buffer_x;
  if (abs(joy_x) < 5)
    joy_x = 0;

  // if ((joy_y_axis-prev_joy_y_reading)>ss_buffer) joy_y_axis=(prev_joy_y_reading+ss_buffer);
  // else if ((joy_y_axis-prev_joy_y_reading)<-ss_buffer) joy_y_axis=(prev_joy_y_reading-ss_buffer);

  buffer_y = (joy_y - prev_joy_y_reading) / 5;
  joy_y = prev_joy_y_reading + buffer_y;
  if (abs(joy_y) < 5)
    joy_y = 0;

  // if ((accel_x_axis-prev_accel_x_reading)>ss_buffer) accel_x_axis=(prev_accel_x_reading+ss_buffer);
  // else if ((accel_x_axis-prev_accel_x_reading)<-ss_buffer) accel_x_axis=(prev_accel_x_reading-ss_buffer);

  buffer_z = (accel_x - prev_accel_x_reading) / 2;
  accel_x = prev_accel_x_reading + buffer_z;
  if (abs(accel_x) < 5)
    accel_x = 0;

  // Serial.print(joy_x_axis);Serial.print(" ___ ");Serial.println(joy_y_axis);

  prev_joy_x_reading = joy_x;
  prev_joy_y_reading = joy_y;
  prev_accel_x_reading = accel_x;

  // FloatPoint fp;
  // fp.x = 0.0;
  // fp.y = 0.0;
  // fp.z = 0.0;

  fp.x = joy_x + current_steps.x;
  fp.y = joy_y + current_steps.y;
  if (AUX_ON)
    fp.z = accel_x + current_steps.z;

  set_target(fp.x, fp.y, fp.z);
  feedrate_micros = calculate_feedrate_delay_2();
}

void applyjoymovebuffer_linear() {

  // control max speeds of the axis
  joy_y_axis = map(joy_y_axis, -90, 90, -35, 35); //

  // slow down changes to avoid sudden stops and starts

  int ss_buffer = 1;

  if ((joy_x_axis - prev_joy_x_reading) > ss_buffer)
    joy_x_axis = (prev_joy_x_reading + ss_buffer);
  else if ((joy_x_axis - prev_joy_x_reading) < -ss_buffer)
    joy_x_axis = (prev_joy_x_reading - ss_buffer);

  if ((joy_y_axis - prev_joy_y_reading) > ss_buffer)
    joy_y_axis = (prev_joy_y_reading + ss_buffer);
  else if ((joy_y_axis - prev_joy_y_reading) < -ss_buffer)
    joy_y_axis = (prev_joy_y_reading - ss_buffer);

  if ((accel_x_axis - prev_accel_x_reading) > ss_buffer)
    accel_x_axis = (prev_accel_x_reading + ss_buffer);
  else if ((accel_x_axis - prev_accel_x_reading) < -ss_buffer)
    accel_x_axis = (prev_accel_x_reading - ss_buffer);

  // Serial.print(joy_x_axis);Serial.print(" ___ ");Serial.println(joy_y_axis);

  prev_joy_x_reading = joy_x_axis;
  prev_joy_y_reading = joy_y_axis;
  prev_accel_x_reading = accel_x_axis;

  float x = joy_x_axis + current_steps.x;
  float y = joy_y_axis + current_steps.y;
  float z = accel_x_axis + current_steps.z;

  set_target(x, y, z);
  feedrate_micros = calculate_feedrate_delay_2();
}

void nc_sleep() {
  // Disable motor if joystick movement exceeds 15 units in any direction
  bool isActive = abs(joy_x_axis) > 15 || abs(joy_y_axis) > 15;
  digitalWrite(MOTOR_EN, isActive ? LOW : HIGH);
}

void updateMotorVelocities2() // Happens  20 times a second
{
  // limit speeds
  float motormax0 = PAN_MAX_JOG_STEPS_PER_SEC / 20000.0;
  float motormax1 = TILT_MAX_JOG_STEPS_PER_SEC / 20000.0;
  float motormax2 = AUX_MAX_JOG_STEPS_PER_SEC / 20000.0;
  if (motormax2 > 0.75)
    motormax2 = .75; // limits max speed during joy to reduce vibration

  // accelerations - accumulator limit is is 65553. Loop is 20Hz.   If we want zero to max to be 1 sec, we choose
  // example 1 If we want zero to max to be 1 sec, we choose (65535/20)/1.0 =3276.75 this is the max per cycle.
  // example 2 If we want zero to max to be 2 sec,(65535/20)/2.0 =1638.375 this is the max per cycle.

  float accelmax0 = (65535.0 / 20.0) / 1.0;
  float accelmax1 = (65535.0 / 20.0) / 1.0;
  float accelmax2 = (65535.0 / 20.0) / 1.0;
  // could also make accel dynamic based on velocity - decelerate faster when going fast - have to make sure we don't create hyperbole

  // exponential curve
  float joy_x = (static_cast<float>(joy_x_axis) * joy_x_axis * joy_x_axis) / 10000.0;
  float joy_y = (static_cast<float>(joy_y_axis) * joy_y_axis * joy_y_axis) / 10000.0;
  float accel_x = (static_cast<float>(accel_x_axis) * accel_x_axis * accel_x_axis)  / 10000.0;

  // record last speed for compare, multiply by direction to get signed value
  float signedlastMotorMoveSpeed0 = motors[0].dir ? motors[0].nextMotorMoveSpeed : -motors[0].nextMotorMoveSpeed;
  float signedlastMotorMoveSpeed1 = motors[1].dir ? motors[1].nextMotorMoveSpeed : -motors[1].nextMotorMoveSpeed;
  float signedlastMotorMoveSpeed2 = motors[2].dir ? motors[2].nextMotorMoveSpeed : -motors[2].nextMotorMoveSpeed;

  // set the accumulator value for the 1/20th second move - this is our accumulator value
  float signedMotorMoveSpeedTarget0 = joy_x * 655.3 * motormax0;
  float signedMotorMoveSpeedTarget1 = joy_y * 655.3 * motormax1;
  float signedMotorMoveSpeedTarget2 = accel_x * 655.3 * motormax2;

  // pan accel
  if (signedMotorMoveSpeedTarget0 != signedlastMotorMoveSpeed0)
  {
    if ((signedMotorMoveSpeedTarget0 > signedlastMotorMoveSpeed0) && ((signedMotorMoveSpeedTarget0 - signedlastMotorMoveSpeed0) > accelmax0)) // accel
    {
      signedMotorMoveSpeedTarget0 = signedlastMotorMoveSpeed0 + accelmax0;
    }

    else if ((signedMotorMoveSpeedTarget0 < signedlastMotorMoveSpeed0) && ((signedlastMotorMoveSpeed0 - signedMotorMoveSpeedTarget0) > accelmax0)) // decel
    {
      signedMotorMoveSpeedTarget0 = signedlastMotorMoveSpeed0 - accelmax0;
    }
  }
  // tilt accel
  if (signedMotorMoveSpeedTarget1 != signedlastMotorMoveSpeed1)
  {
    if ((signedMotorMoveSpeedTarget1 > signedlastMotorMoveSpeed1) && ((signedMotorMoveSpeedTarget1 - signedlastMotorMoveSpeed1) > accelmax1)) // accel
    {
      signedMotorMoveSpeedTarget1 = signedlastMotorMoveSpeed1 + accelmax1;
    }

    else if ((signedMotorMoveSpeedTarget1 < signedlastMotorMoveSpeed1) && ((signedlastMotorMoveSpeed1 - signedMotorMoveSpeedTarget1) > accelmax1)) // decel
    {
      signedMotorMoveSpeedTarget1 = signedlastMotorMoveSpeed1 - accelmax1;
    }
  }
  // aux accel
  if (signedMotorMoveSpeedTarget2 != signedlastMotorMoveSpeed2)
  {
    if ((signedMotorMoveSpeedTarget2 > signedlastMotorMoveSpeed2) && ((signedMotorMoveSpeedTarget2 - signedlastMotorMoveSpeed2) > accelmax2)) // accel
    {
      signedMotorMoveSpeedTarget2 = signedlastMotorMoveSpeed2 + accelmax2;
    }

    else if ((signedMotorMoveSpeedTarget2 < signedlastMotorMoveSpeed2) && ((signedlastMotorMoveSpeed2 - signedMotorMoveSpeedTarget2) > accelmax2)) // decel
    {
      signedMotorMoveSpeedTarget2 = signedlastMotorMoveSpeed2 - accelmax2;
    }
  }
  digitalWrite(MOTOR0_STEP, !digitalRead(MOTOR0_STEP));
  lcd.at(1,1,digitalRead(MOTOR_EN));
  lcd.at(2,1,signedMotorMoveSpeedTarget0);
  lcd.at(2,12,signedMotorMoveSpeedTarget1);

  motors[0].nextMotorMoveSpeed = abs(signedMotorMoveSpeedTarget0); // top is 65535
  motors[1].nextMotorMoveSpeed = abs(signedMotorMoveSpeedTarget1); // top is 65535
  motors[2].nextMotorMoveSpeed = abs(signedMotorMoveSpeedTarget2); // top is 65535

  for (int mot = 0; mot < 3; mot++)
  {
    if (motors[mot].nextMotorMoveSpeed > 0)
      bitSet(motorMoving, mot);
    else
      bitClear(motorMoving, mot);
  }

  motors[0].dir = (signedMotorMoveSpeedTarget0 > 0) ? 1 : 0;
  motors[1].dir = (signedMotorMoveSpeedTarget1 > 0) ? 1 : 0;
  motors[2].dir = (signedMotorMoveSpeedTarget2 > 0) ? 1 : 0;

  *motorAccumulator[0] = 65535;
  *motorAccumulator[1] = 65535;
  *motorAccumulator[2] = 65535;

  // This is just to get us into the loop in the interrupt for each motor to check the test.
  motorMoveSteps0 = 32000;
  motorMoveSteps1 = 32000;
  motorMoveSteps2 = 32000;

  nextMoveLoaded = true;
}
