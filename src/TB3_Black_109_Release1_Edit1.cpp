/*

 =========================================
Main Program
 =========================================

*/

#include "TB3_Camera_Control.h"
#include "TB3_IO_ISR.h"
#include "NHDLCD9.h"
#include "WiiNunchuck3.h"
#include "Structs.h"
#include "TB3_Motor_Control.h"
#include "TB3_EEPROM.h"
#include "TB3_Stepper.h"
#include "TB3_Setup.h"
#include "TB3_PANO.h"
#include "TB3_LCD_Buttons.h"
#include "TB3_Nunchuck.h"
#include "TB3_InShootMenu.h"
#include "TB3_Goto_Position.h"
#include "TB_DF.h"

NHDLCD9 lcd(4, 2, 16); // desired pin, rows, cols   //BB for LCD

// Global Parameters
const uint8_t SETUPMENU = 7;
const bool DEBUG = 0;       //
const bool DEBUG_MOTOR = 0; //
const bool DEBUG_PANO = 0;
const bool DEBUG_GOTO = 0;
const uint8_t MAX_MOVE_POINTS = 3;
const uint32_t VIDEO_FEEDRATE_NUMERATOR = 375; // Set this for 42000, or 375 for faster calc moves
const uint16_t PAN_MAX_JOG_STEPS_PER_SEC = 10000;
const uint16_t TILT_MAX_JOG_STEPS_PER_SEC = 10000;
// #define AUX_MAX_JOG_STEPS_PER_SEC 15000.0 //this is defined in the setup menu now.

// Main Menu Ordering

const uint8_t REG2POINTMOVE = 0;
const uint8_t REV2POINTMOVE = 1;
const uint8_t REG3POINTMOVE = 2;
const uint8_t REV3POINTMOVE = 3;
const uint8_t PANOGIGA = 4;
const uint8_t PORTRAITPANO = 5;

const uint8_t AUXDISTANCE = 99;

// Interval Options
const uint8_t VIDEO_INTVAL = 2;
const uint8_t EXTTRIG_INTVAL = 3;
const uint8_t MIN_INTERVAL_STATIC_GAP = 3; // min gap between interval and static time
// #define STOPMOT //not used

// TB3 section - Black or Orange Port Mapping for Step pins on Stepper Page
const uint8_t MOTORS = 3;
const uint8_t MOTOR0_STEP = 5;
const uint8_t MOTOR1_STEP = 6;
const uint8_t MOTOR2_STEP = 7;
const uint8_t MOTOR0_DIR = 8;
const uint8_t MOTOR1_DIR = 9;
const uint8_t MOTOR2_DIR = 10;
const uint8_t MOTOR_EN = A3;
const uint8_t MOTOR_EN2 = 11;
const uint8_t MS1 = A1;
const uint8_t MS2 = A2;
const uint8_t MS3 = A2;
#define CAMERA_PIN 12         // drives tip of 2.5 mm connector
#define FOCUS_PIN 13          // drives  middle of 2.5mm connector
#define STEPS_PER_DEG 444.444 // 160000 MS per 360 degees = 444.4444444

/*
STEPS_PER_INCH_AUX for various motors with 17 tooth final gear on 5mm pitch belt
Phidgets 99:1	95153
Phidgets 27:1	25676
Phidgets 5:1	4955
20:1 Ratio	19125
10:1 Ratio	9562
*/

uint16_t STEPS_PER_INCH_AUX = 19125;  //
const uint16_t MAX_AUX_MOVE_DISTANCE = 311; //(31.1 inches)
// end TB3 section

unsigned long build_version = 10951;    // this value is compared against what is stored in EEPROM and resets EEPROM and setup values if it doesn't match
unsigned int intval = 2;                // seconds x10  - used for the interval prompt and display
unsigned long interval = 2000;          // calculated and is in ms
unsigned int camera_fired = 0;          // number of shots fired
unsigned int camera_moving_shots = 200; // frames for new duration/frames prompt
unsigned int camera_total_shots = 0;    // used at the end target for camera fired to compare against
unsigned int overaldur = 20;            // seconds now for video only
unsigned int prefire_time = 1;          // currently hardcoded here to .1 second - this powers up motor early for the shot
unsigned int rampval = 50;
unsigned int static_tm = 1; // new variable
unsigned int lead_in = 1;
unsigned int lead_out = 1;
unsigned int start_delay_sec = 0;

// External Interrupt Variables
volatile int state = 0;                  // new variable for interrupt
volatile boolean changehappened = false; // new variable for interrupt
long shuttertimer_open = 0;
long shuttertimer_close = 0;
boolean ext_shutter_open = false;
int ext_shutter_count = 0;
int ext_hdr_shots = 1; // this is how many shots are needed before moving - leave at one for normal shooting - future functionality with external

// Start of variables for Pano Mode
unsigned int P2PType = 1; // 0 = no accel, 1= accel
unsigned int PanoPostMoveDelay = 200;

// 3 Point motor routine values
float motor_steps_pt[MAX_MOVE_POINTS][MOTORS];                          // 3 total points.   Start point is always 0.0
float percent;                                                          //% through a leg
unsigned int keyframe[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}}; // this is basically the keyframes {start, end of rampup, start or rampdown, end}   - doesn't vary by motor at this point
float linear_steps_per_shot[MOTORS] = {0.0, 0.0, 0.0};                  //{This is for the calculated or estimated steps per shot in a segment for each motor
float ramp_params_steps[MOTORS] = {0.0, 0.0, 0.0};                      // This is to calc the steps at the end of rampup for each motor.  Each array value is for a motor

// Program Status Flags
boolean Program_Engaged = false;
boolean Shot_Sequence_Engaged = false;
boolean Prefire_Engaged = false;
boolean Static_Time_Engaged = false;
boolean IO_Engaged = false;
boolean Move_Engaged = false;
boolean Interrupt_Fire_Engaged = false;

// New Powersave flags
/*Power Save explanation
We can power up and power down the Pan Tilt motors together.  We can power up and power down the Aux motor port as well.  We see three levels of power saving:
1)  None - Motors are always on - for VFX work where power isn't a factor and precision is most important.  Motors will get warm here on hot days.
2)  Low - only at the end of program
3)  Standard - Power up the motors for the shooting time (all the time we hold the trigger down), and move, power down between shots.
4)  High - Only power on for motor moves, turn off the motors when we reach the shooting position.
    We are powered down for the shot and only power on for moves. This saves a ton of battery for long astro shots.
    We do lose microstep resolution for this, but it usually is not visible.   We could be off by as much as 8/16 mircosetps for a shot or 0.018 degrees - Really small stuff!  Try this mode out!
*/

// CVariables that are set during the Setup Menu store these in EEPROM
unsigned int POWERSAVE_PT;  // 1=None - always on  2 - low   3=standard    4=High
unsigned int POWERSAVE_AUX; // 1=None - always on  2 - low   3=standard    4=High
byte AUX_ON;                // 1=Aux Enabled, 2=Aux disabled
byte PAUSE_ENABLED;         // 1=Pause Enabled, 0=Pause disabled
boolean REVERSE_PROG_ORDER; // Program ordering 0=normal, start point first. 1=reversed, set end point first to avoid long return to start
boolean MOVE_REVERSED_FOR_RUN = 0;
unsigned int LCD_BRIGHTNESS_DURING_RUN; // 0 is off 8 is max
uint16_t AUX_MAX_JOG_STEPS_PER_SEC; // value x 1000  20 is the top or 20000 steps per second.
byte AUX_REV;                           // 1=Aux Enabled, 2=Aux disabled

// control variable, no need to store in EEPROM - default and setup during shot
unsigned int progstep = 0;           // used to define case for main loop
boolean progstep_forward_dir = true; // boolean to define direction of menu travel to allow for easy skipping of menus
unsigned int progtype = 0;           // updownmenu selection
int inprogtype = 0;                  // updownmenu selection during shoot

unsigned int first_time = 1;         // variable to help with LCD dispay variable that need to show one time

unsigned int max_prefire;
unsigned int program_progress_2PT = 1; // Lead in, ramp, linear, etc for motor routine case statement
unsigned int program_progress_3PT = 1; // phase 1, phase 2
unsigned long interval_tm = 0;         // mc time to help with interval comparison
unsigned long interval_tm_last = 0;    // mc time to help with interval comparison
unsigned int lcd_dim_tm = 10;
unsigned long diplay_last_tm = 0;
unsigned int prompt_time = 500; // in ms for delays of instructions
// unsigned int  prompt_time=350; // for faster debugging
int prompt_delay = 0; // to help with joystick reads and delays for inputs - this value is set during joystick read and executed later in the loop
int prompt_val;
int reviewprog = 1;

unsigned long start_delay_tm = 0; // ms timestamp to help with delay comparison

int sequence_repeat_type = 1;  // 1 Defaults - Run Once, 0 Continuous Loop,  -1 Continuous Forward

// remote and interface variables

float joy_x_axis;

float joy_y_axis;
float accel_x_axis;

int PanStepCount;
int TiltStepCount;

int z_button = 0;
int c_button = 0;
unsigned int joy_y_lock_count = 0;
unsigned int joy_x_lock_count = 0;
int prev_accel_x_reading = 0;
int CZ_Button_Read_Count = 0;
boolean CZ_Released = true;
boolean C_Released = true;
boolean Z_Released = true;
unsigned int NCReadMillis = 42; // frequency at which we read the nunchuck for moves  1000/24 = 42  1000/30 = 33
long NClastread = 1000;         // control variable for NC reads cycles

// Stepper Setup
uint32_t feedrate_micros = 0;

FloatPoint fp;

FloatPoint current_steps;
FloatPoint target_steps;
FloatPoint delta_steps;

// End setup of Steppers

// supported boards
#define ARDUINO 1
#define ARDUINOMEGA 2

// eMotimo TB3 - Set this PINOUT_VERSION 3 for TB3 Orange (Uno)
// eMotimo TB3 - Set this PINOUT_VERSION 4 for TB3 Black (MEGA)
const uint8_t PINOUT_VERSION = 4;

/*
  This is PINOUT_VERSION 1

  channel 5
        PIN  22   step
        PIN  23   direction
  channel 6
        PIN  24   step
        PIN  25   direction
  channel 7
        PIN  26   step
        PIN  27   direction
  channel 8
        PIN  28   step
        PIN  29   direction
*/

// detect board type
#define BOARD ARDUINOMEGA

#define SERIAL_DEVICE Serial

#define PIN_ON(port, pin) \
  {                       \
    port |= pin;          \
  }
#define PIN_OFF(port, pin) \
  {                        \
    port &= ~pin;          \
  }

const uint8_t MOTOR_COUNT = 4;

const uint8_t TIME_CHUNK = 50;
#define SEND_POSITION_COUNT 20000

// Start TB3 Black Port Mapping

uint8_t MOTOR0_STEP_PORT = PORTE;
const uint8_t MOTOR0_STEP_PIN = B00001000; // Pin 5 PE3

uint8_t MOTOR1_STEP_PORT = PORTH;
const uint8_t MOTOR1_STEP_PIN = B00001000; // Pin  6 PH3

uint8_t MOTOR2_STEP_PORT = PORTH;
const uint8_t MOTOR2_STEP_PIN = B00010000; // Pin 7 PH4

uint8_t MOTOR3_STEP_PORT = PORTC;    //  Map this to pin 30 PC7 on the Mega board for debug
const uint8_t MOTOR3_STEP_PIN = B10000000; //
// End TB3 Black Port Mapping


/*
 Motor data.
 */

uint16_t motorAccumulator0;
uint16_t motorAccumulator1;
uint16_t motorAccumulator2;
uint16_t motorAccumulator3;

uint16_t *motorAccumulator[MOTOR_COUNT] =
    {
        &motorAccumulator0,
        &motorAccumulator1,
        &motorAccumulator2,
        &motorAccumulator3,

};

uint16_t motorMoveSpeed0;
uint16_t motorMoveSpeed1;
uint16_t motorMoveSpeed2;
uint16_t motorMoveSpeed3;

uint16_t *motorMoveSpeed[MOTOR_COUNT] =
    {
        &motorMoveSpeed0,
        &motorMoveSpeed1,
        &motorMoveSpeed2,
        &motorMoveSpeed3,

};


uint16_t motorMoveSteps0;
uint16_t motorMoveSteps1;
uint16_t motorMoveSteps2;
uint16_t motorMoveSteps3;

uint16_t *motorMoveSteps[MOTOR_COUNT] =
    {
        &motorMoveSteps0,
        &motorMoveSteps1,
        &motorMoveSteps2,
        &motorMoveSteps3,
};

volatile boolean nextMoveLoaded;

byte motorMoving = 0;

boolean maxVelLimit = false;

Motor motors[MOTOR_COUNT];

// End of DFVars

/*
 =========================================
 Setup functions
 =========================================
*/

void setup()
{

  // setup motor pins
  pinMode(MOTOR0_STEP, OUTPUT);
  pinMode(MOTOR0_DIR, OUTPUT);
  pinMode(MOTOR1_STEP, OUTPUT);
  pinMode(MOTOR1_DIR, OUTPUT);
  pinMode(MOTOR2_STEP, OUTPUT);
  pinMode(MOTOR2_DIR, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_EN2, OUTPUT);
  digitalWrite(MOTOR_EN, HIGH);  // LOW Enables output, High Disables
  digitalWrite(MOTOR_EN2, HIGH); // LOW Enables output, High Disables

  // setup camera pins
  pinMode(CAMERA_PIN, OUTPUT);
  pinMode(FOCUS_PIN, OUTPUT);

  digitalWrite(CAMERA_PIN, LOW);
  digitalWrite(FOCUS_PIN, LOW);

  setup_IO();

  pinMode(A0, INPUT); // this is for the voltage reading

  // Setup LCD
  lcd.setup();
  delay(100);
  draw(0, 1, 2); // Setup Complete
  draw(1, 2, 1); // Version Number

  lcd.contrast(50);
  lcd.cursorOff();
  lcd.bright(4);

  delay(prompt_time * 2);
  lcd.empty();
  delay(100);

  draw(2, 1, 1); // Connect Joystick

  // Setup Serial Connection

  // if (DEBUG) Serial.begin(115200);
  // else Serial.begin(57600);
  Serial.begin(57600);
  Serial.println("Opening Serial Port");

  // Handle EEPROM Interaction and upgrades

  // Check to see if our hardcoded build version set in progam is different than what was last put in EEPROM - detect upgrade.
  if (build_version != check_version())
  { // 4 byte string that now holds the build version.
    if (DEBUG)
      Serial.println(check_version());
    if (DEBUG)
      Serial.println("Upgrading Memory");
    write_defaults_to_eeprom_memory(); // these are for setting for last shot
    set_defaults_in_setup();           // this is for our setup values that should only be defaulted once.
    // review_RAM_Contents();
  }
  else
  { // load last setting into memory - no upgrade
    if (DEBUG)
      Serial.println("Restoring EEPROM Values");
    restore_from_eeprom_memory();
    // review_RAM_Contents();
  }
  // End Setup of EEPROM

  // begin  Setup for Nunchuck
  Nunchuck.init(0);
  delay(50);
  for (int reads = 1; reads < 17; reads++)
  {
    Nunchuck.getData();
    // Nunchuck.printData();
    lcd.at(2, reads, "+");
    if (abs(Nunchuck.joyx() - 127) > 60 || abs(Nunchuck.joyy() - 127) > 60)
    {
      lcd.empty();
      draw(57, 1, 1); // lcd.at(1,1,"Center Joystick");
      reads = 1;
    }
    delay(10);
  }

  calibrate_joystick(Nunchuck.joyx(), Nunchuck.joyy());

  // end  Setup for Nunchuk
  lcd.empty();

  // Setup Motors
  init_steppers();

} // end of setup

void loop()
{ // Main Loop
  while (1)
  { // use debugging WHEN HIT here for monitoring - {sequence_repeat_type},{progstep},{progtype},{camera_fired}
    switch (progstep)
    {

      // start of 2 point SMS/Video routine
    case 0: //
      Choose_Program();

      break;

    case 1:                 // Move to Start Point
      Move_to_Startpoint(); // don't jump in this loop by accident
      break;

    case 2:               // Move to End Point
      Move_to_Endpoint(); // don't jump in this loop by accident
      break;

    case 3: //  Set Camera Interval
      Set_Cam_Interval();
      break;

    case 4: //
      Set_Duration();
      break;

    case 5: //  	    Static Time

      if (intval == VIDEO_INTVAL)
      { // don't show this for video
        if (progstep_forward_dir)
          progstep_forward(); // skip the menu, go forward
        else
          progstep_backward(); // skip the menu, go backward
      }
      else
        Set_Static_Time(); // not needed for video

      break;

    case 6: //

      Set_Ramp();

      break;

    case 7: //  Lead in and lead out

      if (intval == VIDEO_INTVAL)
      { // don't show this for video
        if (progstep_forward_dir)
        {
          Calculate_Shot(); //
          progstep_forward();
        } // skip the menu, go forward
        else
          progstep_backward(); // skip the menu, go backward
      }

      else
        Set_LeadIn_LeadOut(); //  not needed for video

      break;

    case 8: //  Set Shot Type
      if (intval != VIDEO_INTVAL)
      { // skip for non video
        if (progstep_forward_dir)
        {
          progstep_forward(); // skip the menu, go forward
        }
        else
          progstep_backward(); // skip the menu, go backward
      }

      else
        Set_Shot_Repeat();
      break;

    case 9:             //  review and confirm
      Review_Confirm(); // also has the delay start options here
      break;

      // end of the two point move

      // start of the three point move
    case 100:
      Choose_Program();
      break;

    case 101: // Move Point 0
      Move_to_Point_X(0);
      break;

    case 102: // Move Point 1
      Move_to_Point_X(1);
      break;

    case 103: // Move Point 2
      Move_to_Point_X(2);
      break;

    case 104: //  Set Camera Interval
      Set_Cam_Interval();
      break;

    case 105: //
      Set_Duration();
      break;

    case 106: //
      Set_Static_Time();
      break;

    case 107: //
      Set_Ramp();
      break;

    case 108: //
      Set_LeadIn_LeadOut();
      break;

    case 109: //  review and confirm
      Review_Confirm();
      break;
      // end of the three point move

      // start of pano Mode

      //  define field of view
      // We want to know how wide and tall our field of view is in steps so we can get our overlap right.  Anytime you zoom or change lenses, this need to be redefined
      // This should be a 10 seconds process to define by specifying corners
      // Step 1 - Put a point in the upper right corner - set zeros, pan up and right to hit same point with lower left corner of viewfinder
      // Display values  - write to ram - use these values

    case 200: //
      Choose_Program();
      break;

    case 201:             //
      Move_to_Point_X(0); // move to sharp point
      break;

    case 202: //
      Set_angle_of_view();
      break;

    case 203: //
      Define_Overlap_Percentage();
      break;

    case 204: //
      Move_to_Point_X(0);
      break;

    case 205: //
      Move_to_Point_X(1);
      break;

    case 206: //
      Set_Static_Time();
      break;

    case 207: //
      Pano_Review_Confirm();
      break;
      // end of Pano Mode

      // start of Portrait Pano Method

      //  define field of view
      // We want to know how wide and tall our field of view is in steps so we can get our overlap right.  Anytime you zoom or change lenses, this need to be redefined
      // This should be a 10 seconds process to define by specifying corners
      // Step 1 - Put a point in the upper right corner - set zeros, pan up and right to hit same point with lower left corner of viewfinder
      // Display values  - write to ram - use these values

    case 210: //
      Choose_Program();
      break;

    case 211:             //
      Move_to_Point_X(0); // move to sharp point
      break;

    case 212: //
      Set_angle_of_view();
      break;

    case 213: //
      Define_Overlap_Percentage();
      break;

    case 214:             //
      Move_to_Point_X(0); // set subject point
      break;

    case 215:              //
      Set_PanoArrayType(); // this sets variable that define how we move camera - load the appropriate array.
      break;

    case 216: //
      Set_Static_Time();
      break;

    case 217: //
      Pano_Review_Confirm();
      break;
      // end of Pano Mode

      // start of entered distance on aux mode
    case 300: //
      Choose_Program();
      break;

    case 301: // Move to Start Point
      Move_to_Startpoint();
      break;

    case 302: // Move to End Point
      Enter_Aux_Endpoint();
      break;

    case 303: //  Set Camera Interval
      Set_Cam_Interval();
      break;

    case 304: //
      Set_Duration();
      break;

    case 305: //
      Set_Static_Time();
      break;

    case 306: //
      Set_Ramp();
      break;

    case 307: //
      Set_LeadIn_LeadOut();
      break;

    case 308: //  review and confirm
      Review_Confirm();
      break;

      // end entered distance mode

      // start of setup

    case 901: // AUX_ON
      Setup_AUX_ON();
      break;

    case 902: // PAUSE_ENABLED
      Setup_PAUSE_ENABLED();
      break;

    case 903: // POWERSAVE_PT
      Setup_POWERSAVE_PT();
      break;

    case 904: //  POWERSAVE_AUX
      Setup_POWERSAVE_AUX();
      break;

    case 905: //  LCD Bright
      Setup_LCD_BRIGHTNESS_DURING_RUN();
      break;

    case 906: //  Aux Motor Max Speed
      Setup_Max_AUX_Motor_Speed();
      break;

    case 907: //  LCD Bright
      Setup_AUX_Motor_DIR();
      break;

    case 908: //  Exit
      delay(100);
      break;

      // end of setup

      // start of in program menu options

    case 1001: // AUX_ON
      InProg_Select_Option();
      break;

      // end of in program menu

    case 50: // loop for SMS

      // //Step 1 if internal interval.Kick off the shot sequence. This happens once per camera shot.
      if ((intval > 3) && (Program_Engaged) && !(Shot_Sequence_Engaged) && ((millis() - interval_tm) > interval))
      {
        interval_tm_last = interval_tm; // just used for shot timing comparison
        interval_tm = millis();         // start the clock on our shot sequence

        if (DEBUG)
        {
          Serial.print("trueinterval: ");
          Serial.print(interval_tm - interval_tm_last);
          Serial.print(";");
        }
        Interrupt_Fire_Engaged = false; // clear this flag to avoid rentering this routine
        Shot_Sequence_Engaged = true;   //
        Prefire_Engaged = true;         //
        IO_Engaged = true;              //
        digitalWrite(FOCUS_PIN, HIGH);  // for longer shot interval, wake up the camera

        if (POWERSAVE_PT < 4)
          enable_PT(); // don't power on for shot for high power saving
        if (AUX_ON && POWERSAVE_AUX < 4)
          enable_AUX(); // don't power on for shot for high power saving
      }

      // Step 1 if external triggering. This happens once per camera shot.
      if ((Program_Engaged) && !(Shot_Sequence_Engaged) && (intval == EXTTRIG_INTVAL) && Interrupt_Fire_Engaged)
      {
        interval_tm_last = interval_tm; // just used for shot timing comparison

        interval_tm = millis(); // start the clock on our shot sequence
        if (DEBUG)
        {
          Serial.print("trueinterval: ");
          Serial.print(interval_tm - interval_tm_last);
          Serial.print(";");
        }
        Interrupt_Fire_Engaged = false; // clear this flag to avoid rentering this routine

        Shot_Sequence_Engaged = true;  //
        Prefire_Engaged = true;        //
        IO_Engaged = true;             //
        digitalWrite(FOCUS_PIN, HIGH); // for longer shot interval, wake up the camera

        if (POWERSAVE_PT < 4)
          enable_PT(); // don't power on for shot for high power saving
        if (AUX_ON && POWERSAVE_AUX < 4)
          enable_AUX(); // don't power on for shot for high power saving
      }

      // End our prefire - check that we are in program active,shot cycle engaged, and prefire engaged and check against our prefire time
      // If so set prefire flag off, static flag on, fire camera for static time value, update the display

      if ((Shot_Sequence_Engaged) && (Prefire_Engaged) && ((millis() - interval_tm) > prefire_time * 100))
      {

        Prefire_Engaged = false;
        if (DEBUG)
        {
          Serial.print("PreDoneAt ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }

        Static_Time_Engaged = true;
        // Fire Camera
        if (intval != 3)
          fire_camera((long)static_tm * 100); // start shutter sequence
        camera_fired++;
      }

      // End out static time - check that we are in an program active and static time,  Shutter not engaged, check shot cycle time agains prefire+statictime
      // If so remove flags from Static Time Engaged and IO engaged, Turn off I/O port, set flags for motors moving, move motors
      // move motors - figure out delays.   Long delays mean really slow - choose the minimum of the calculated or a good feedrate that is slow

      // if (Program_Engaged && Shot_Sequence_Engaged && Static_Time_Engaged && !Shutter_Signal_Engaged && ((millis() - interval_tm) > (prefire_time*100+static_tm*100)) ) {
      if (Shot_Sequence_Engaged && Static_Time_Engaged && !camera_shutter() && ((millis() - interval_tm) > (prefire_time * 100 + static_tm * 100)))
      { // removed requirement for Program Engaged for external interrupt

        Static_Time_Engaged = false; // Static Time Engaged is OFF
        IO_Engaged = false;          // IO Engaged is off
        // digitalWrite(IO_2, LOW); //Use this as the iterrupt
        // digitalWrite(IO_3, LOW);  //Turn off Pin 3
        // Serial.print("IO3_off"); //Serial.println(millis()-interval_tm);

        // Move the motors - each motor move is calculated by where we are in the sequence - we still call this for lead in and lead out - motors just don't move

        Move_Engaged = true; // move motors
        if (DEBUG_MOTOR)
        {
          Serial.print("MoveStart ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }
        move_motors();
        if (DEBUG_MOTOR)
        {
          Serial.print("Moveend ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }

        // Turn off the motors if we have selected powersave 3 and 4 are the only ones we want here
        if (POWERSAVE_PT > 2)
          disable_PT();
        if (POWERSAVE_AUX > 2)
          disable_AUX(); //

        // Update display
        if (intval != 3)
          display_status(); // update after shot complete to avoid issues with pausing

        Shot_Sequence_Engaged = false; // Shot sequence engaged flag is is off - we are ready for our next
        Interrupt_Fire_Engaged = false;
        // CZ_Button_Read_Count=0;
        // InterruptAction_Reset(); //enable the external interrupts to start a new shot
        if (DEBUG)
        {
          Serial.println("EOL");
        }
      }

      if (camera_moving_shots > 0 && camera_fired >= camera_total_shots)
      { // end of program
        lcd.empty();
        draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
        Program_Engaged = false;
        if (POWERSAVE_PT > 1)
          disable_PT(); //  low, standard, high, we power down at the end of program
        if (POWERSAVE_AUX > 1)
          disable_AUX(); // low, standard, high, we power down at the end of program
        delay(prompt_time * 2);
        progstep = 90;
        first_time = 1;
      }

      // This portion always runs in empty space of loop.

      NunChuckQuerywithEC();
      NunChuckjoybuttons();
      Check_Prog(); // look for button presses
      // if (CZ_Button_Read_Count>10 && intval==EXTTRIG_INTVAL ) Interrupt_Fire_Engaged=true; // manual trigger
      // if (PAUSE_ENABLED && CZ_Button_Read_Count>10 && intval>3 && !Shot_Sequence_Engaged ) Pause_Prog(); //pause an SMS program
      if (PAUSE_ENABLED && CZ_Button_Read_Count > 25 && intval > 3 && !Shot_Sequence_Engaged && CZ_Released)
        SMS_In_Shoot_Paused_Menu(); // jump into shooting menu

      break; // break 50

    case 51:

      // main video loop interrupt based.  This runs for 2 point moves only.

      if (progtype == REG2POINTMOVE || progtype == REV2POINTMOVE)
      {
        synched3AxisMove_timed(motor_steps_pt[2][0], motor_steps_pt[2][1], motor_steps_pt[2][2], float(overaldur), float(rampval / 100.0));
        if (maxVelLimit)
        { // indicates the move is limited to enforce velocity limit on motors)
          lcd.at(2, 1, "Speed Limit");
        }
        // Start us moving
        // interval_tm_last=interval_tm;
        interval_tm = millis();

        startISR1();
        do
        {
          if (!nextMoveLoaded)
          {
            updateMotorVelocities();
          }
        } while (motorMoving);

        stopISR1();

        Serial.print("Video Runtime");
        Serial.println(millis() - interval_tm);

        if (!motorMoving && (sequence_repeat_type == 0))
        { // new end condition for RUN CONTINOUS
          boolean break_continuous = false;
          lcd.empty();
          draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
          Program_Engaged = false;
          for (int i = 0; i < 30; i++)
          {
            NunChuckQuerywithEC();
            NunChuckjoybuttons();
            Check_Prog(); // look for button presses
            if (PAUSE_ENABLED && CZ_Button_Read_Count > 25)
            {
              break_continuous = true;
              lcd.empty();
              lcd.at(1, 1, "Stopping Run");
              lcd.at(2, 1, "Release Buttons");
              do
              {
                NunChuckQuerywithEC();
                NunChuckjoybuttons();

              } while (c_button || z_button);
              progstep = 9;
            }
          }

          // add section to delay here if the delay is set.
          while (start_delay_tm > millis() / 1000L)
          {
            // enter delay routine
            calc_time_remain_start_delay();
            if ((millis() - diplay_last_tm) > 1000)
              display_time(2, 1);
            NunChuckQuerywithEC();
            NunChuckjoybuttons();
            Check_Prog(); // look for long button press
            // if (CZ_Button_Read_Count>20 && !Program_Engaged) {
            //	start_delay_tm=((millis()/1000L)+5); //start right away by lowering this to 5 seconds.
            //	CZ_Button_Read_Count=0; //reset this to zero to start
            // }
          }

          // end start delay

          if (!break_continuous)
            Auto_Repeat_Video(); // only run this if there isn't a break command
          first_time = 1;
        }
        else if (!motorMoving && (sequence_repeat_type == 1))
        { // new end condition for RUN ONCE
          lcd.empty();
          draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
          Program_Engaged = false;
          if (POWERSAVE_PT > 1)
            disable_PT(); //  low, standard, high, we power down at the end of program
          if (POWERSAVE_AUX > 1)
            disable_AUX(); // low, standard, high, we power down at the end of program
          progstep = 90;
          first_time = 1;
          delay(100);
          // NunChuckQuerywithEC();
        }

      } // end interrupt routine driven for 2 points

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // Start video loop for 3 Point moves - this loop does not use the new motor move profile as it needs to calculate each and every segment
      if (progtype == REG3POINTMOVE || progtype == REV3POINTMOVE)
      { // this is regular 3 point move program that can be modified later
        if (DEBUG)
        {
          interval_tm_last = interval_tm;
          interval_tm = millis();
        }
        // Start us moving

        if (DEBUG)
        {
          Serial.print("trueinterval ");
          Serial.print(interval_tm - interval_tm_last);
          Serial.print(";");
        }

        if (DEBUG_MOTOR)
        {
          Serial.print("MoveStart ");
          Serial.print(millis());
          Serial.print(";");
        }

        camera_fired++; // still need this for framing
        move_motors();

        if (DEBUG_MOTOR)
        {
          Serial.print("Moveend ");
          Serial.println(millis());
        }

        if (camera_total_shots > 0 && camera_fired >= camera_total_shots)
        {
          lcd.empty();
          // draw(58,1,1);//lcd.at(1,1,"Program Complete");
          Program_Engaged = false;
          if (POWERSAVE_PT > 1)
            disable_PT(); //  low, standard, high, we power down at the end of program
          if (POWERSAVE_AUX > 1)
            disable_AUX(); // low, standard, high, we power down at the end of program
          delay(prompt_time * 2);
          progstep = 90;
          first_time = 1;
          // delay(100);
          // NunChuckQuerywithEC();
        }

      } // End video loop for 3 Point moves

      break; // break 51 - VIDEO

    case 52: // loop for external interrupt - external triggering

      // New interrupt Flag Checks

      if (changehappened)
      {
        changehappened = false;
        if (!state) // start the clock as the cam shutter witch closed and sense pin, was brought low
        {
          ext_shutter_open = true;
          shuttertimer_open = micros();
          if (DEBUG)
            Serial.print("shuttertimer_a=");
          Serial.print(shuttertimer_open);
        }
        else if (state) // shutter closed - sense pin goes back high - stop the clock and report
        {
          ext_shutter_open = false;
          shuttertimer_close = micros(); // turn on the led / shutter
          ext_shutter_count++;
          if (DEBUG)
            Serial.print(" ext_shutter_count=");
          Serial.print(ext_shutter_count);
          if (DEBUG)
            Serial.print(" shuttertimer_b=");
          Serial.print(shuttertimer_close);
          Serial.print("diff=");
          Serial.println(shuttertimer_close - shuttertimer_open);
        }
      }
      // end interrupt check and flagging

      //  Start of states for external shooting loop

      if ((Program_Engaged) && !(Shot_Sequence_Engaged) && !camera_shutter() && (ext_shutter_open))
      { // start a shot sequence flag

        Shot_Sequence_Engaged = true; //
      }

      if ((Program_Engaged) && (Shot_Sequence_Engaged) && !camera_shutter() && (ext_shutter_open))
      { // fire the camera can happen more than once in a shot sequence with HDR

        if (DEBUG)
        {
          Serial.print("Startshot_at:");
          Serial.print(millis());
          Serial.println(";");
        }

        // Fire Camera
        // don't fire the camera with the timer, just turn on our focus and shutter pins - we will turn them off when we sense the shot is done.
        fire_camera();
        // camera_fired++;
      }

      if (Shot_Sequence_Engaged && camera_shutter() && !(ext_shutter_open))
      { // shutter just closed, stop the camera port and move

        stop_camera();

        if (ext_shutter_count >= ext_hdr_shots)
        { // this is future functionality  - leave at 1 for now

          camera_fired++;
          ext_shutter_count = 0;
          // Move the motors - each motor move is calculated by where we are in the sequence - we still call this for lead in and lead out - motors just don't move

          Move_Engaged = true; // move motors
          if (DEBUG_MOTOR)
          {
            Serial.print("MoveStart ");
            Serial.print(millis() - interval_tm);
            Serial.print(";");
          }
          move_motors();
          if (DEBUG_MOTOR)
          {
            Serial.print("Moveend ");
            Serial.print(millis() - interval_tm);
            Serial.print(";");
          }

          // Turn off the motors if we have selected powersave 3 and 4 are the only ones we want here
          if (POWERSAVE_PT > 2)
            disable_PT();
          if (POWERSAVE_AUX > 2)
            disable_AUX(); //

          // Update display
          display_status(); // update after shot complete to avoid issues with pausing

          Shot_Sequence_Engaged = false; // Shot sequence engaged flag is is off - we are ready for our next
          CZ_Button_Read_Count = 0;
          // InterruptAction_Reset(); //enable the external interrupts to start a new shot
          if (DEBUG)
          {
            Serial.println("EOL");
          }
        }
      }

      if (camera_moving_shots > 0 && camera_fired >= camera_total_shots)
      { // end of program
        lcd.empty();
        draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
        Program_Engaged = false;
        if (POWERSAVE_PT > 1)
          disable_PT(); //  low, standard, high, we power down at the end of program
        if (POWERSAVE_AUX > 1)
          disable_AUX(); // low, standard, high, we power down at the end of program
        delay(prompt_time * 2);
        progstep = 90;
        first_time = 1;
      }
      NunChuckQuerywithEC();
      NunChuckjoybuttons();
      Check_Prog(); // look for button presses
      if (CZ_Button_Read_Count > 10 && intval == EXTTRIG_INTVAL)
        Interrupt_Fire_Engaged = true; // manual trigger
                                       // if (PAUSE_ENABLED && CZ_Button_Read_Count>20 && intval>3 && !Shot_Sequence_Engaged ) Pause_Prog(); //pause an SMS program

      break; // break 52 - end external triggering loop

    case 90: // end of program - offer repeat and reverse options - check the nuncuck
      if (first_time == 1)
      {
        lcd.empty();
        lcd.at(1, 4, "Repeat - C");
        lcd.at(2, 4, "Reverse - Z");
        NunChuckQuerywithEC();
        first_time = 0;
        delay(100);
      }

      // This portion always runs in empty space of loop.

      NunChuckQuerywithEC();
      NunChuckjoybuttons();
      Check_Prog(); // look for button presses
      // add error handling here to prevent accidental starts
      // if (CZ_Button_Read_Count>25  && CZ_Released ) button_actions_end_of_program();  //Repeat or Reverses
      button_actions_end_of_program();
      // delay(1); //don't just hammer on this - query at regular interval

      break; // break 90

    case 250: // loop for Pano

      // Kick off the shot sequence!!!  This happens once per camera shot.
      if ((intval > 2) && (Program_Engaged) && !(Shot_Sequence_Engaged) && ((millis() - interval_tm) > interval))
      {
        interval_tm_last = interval_tm; // just used for shot timing comparison

        interval_tm = millis(); // start the clock on our shot sequence
        if (DEBUG)
        {
          Serial.print("trueinterval: ");
          Serial.print(interval_tm - interval_tm_last);
          Serial.print(";");
        }
        Interrupt_Fire_Engaged = false; // clear this flag to avoid re-entering this routine

        Shot_Sequence_Engaged = true;  //
        Prefire_Engaged = true;        //
        IO_Engaged = true;             //
        digitalWrite(FOCUS_PIN, HIGH); // for longer shot interval, wake up the camera

        // if (POWERSAVE_PT<4)   enable_PT();  //don't power on for shot for high power saving
        // if (AUX_ON && POWERSAVE_AUX<4)   enable_AUX();  //don't power on for shot for high power saving
        enable_PT();
      }

      // End our prefire - check that we are in program active,shot cycle engaged, and prefire engaged and check against our prefire time
      // If so set prefire flag off, static flag on, fire camera for static time value, update the display

      if ((Shot_Sequence_Engaged) && (Prefire_Engaged) && ((millis() - interval_tm) > prefire_time * 100))
      {

        Prefire_Engaged = false;
        if (DEBUG)
        {
          Serial.print("PreDoneAt ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }

        Static_Time_Engaged = true;
        // Fire Camera
        fire_camera((long)static_tm * 100); // start shutter sequence
        camera_fired++;
      }

      // End out static time - check that we are in an program active and static time,  Shutter not engaged, check shot cycle time against prefire+statictime
      // If so remove flags from Static Time Engaged and IO engaged, Turn off I/O port, set flags for motors moving, move motors
      // move motors - figure out delays.   Long delays mean really slow - choose the minimum of the calculated or a good feedrate that is slow

      // if (Program_Engaged && Shot_Sequence_Engaged && Static_Time_Engaged && !Shutter_Signal_Engaged && ((millis() - interval_tm) > (prefire_time*100+static_tm*100)) ) {
      if (Shot_Sequence_Engaged && Static_Time_Engaged && !camera_shutter() && ((millis() - interval_tm) > (prefire_time * 100 + static_tm * 100)))
      { // removed requirement for Program Engaged for external interrupt

        Static_Time_Engaged = false; // Static Time Engaged is OFF
        IO_Engaged = false;          // IO Engaged is off
        // digitalWrite(IO_2, LOW); //Use this as the iterrupt
        // digitalWrite(IO_3, LOW);  //Turn off Pin 3
        // Serial.print("IO3_off"); //Serial.println(millis()-interval_tm);

        // Move the motors - each motor move is calculated by where we are in the sequence - we still call this for lead in and lead out - motors just don't move

        Move_Engaged = true; // move motors
        if (DEBUG_MOTOR)
        {
          Serial.print("MoveStart ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }
        if (DEBUG_PANO)
          Serial.print("progtype ");
        Serial.println(progtype);
        if (progtype == PANOGIGA) // regular pano
        {
          if (P2PType == 0)
          {
            move_motors_pano_basic();
          }
          else if (P2PType == 1)
          {
            move_motors_pano_accel();
          }
        }

        else if (progtype == PORTRAITPANO) // PORTRAITPANO method array load
        {
          if (DEBUG_PANO)
            Serial.print("entered PORTRAITPANO loop");
          move_motors_accel_array();
          delay(PanoPostMoveDelay);
        }

        //
        if (DEBUG_MOTOR)
        {
          Serial.print("Moveend ");
          Serial.print(millis() - interval_tm);
          Serial.print(";");
        }

        // Turn off the motors if we have selected powersave 3 and 4 are the only ones we want here
        // if (POWERSAVE_PT>2)   disable_PT();
        // if (POWERSAVE_AUX>2)   disable_AUX();  //

        //

        if (P2PType == 0)
        {
          Serial.println("finished basic move");
          if (intval != 3)
            display_status(); // update after shot complete to avoid issues with pausing
          Move_Engaged = false;
          Shot_Sequence_Engaged = false; // Shot sequence engaged flag is is off - we are ready for our next
          Interrupt_Fire_Engaged = false;
          CZ_Button_Read_Count = 0;
          // InterruptAction_Reset(); //enable the external interrupts to start a new shot
          if (DEBUG)
          {
            Serial.println("EOL");
          }
        }

      } // end test

      // just have this repeat like we are in loop
      if (P2PType == 1) // acceleration profiles
      {

        if (!nextMoveLoaded)
        {
          updateMotorVelocities(); // finished up the interrupt routine
          // Print_Motor_Params(2);
        }

        // test for completed move
        if (Shot_Sequence_Engaged && Move_Engaged && motorMoving == 0) // motors completed the move
        {
          Serial.println("finished accel move");
          if (intval != 3)
            display_status(); // update after shot complete to avoid issues with pausing
          Move_Engaged = false;
          Shot_Sequence_Engaged = false; // Shot sequence engaged flag is is off - we are ready for our next
          Interrupt_Fire_Engaged = false;
          CZ_Button_Read_Count = 0;
          // InterruptAction_Reset(); //enable the external interrupts to start a new shot
          if (DEBUG)
          {
            Serial.println("EOL");
          }
        }
      }

      if (camera_moving_shots > 0 && camera_fired >= camera_total_shots)
      { // end of program
        lcd.empty();
        draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
        Program_Engaged = false;
        if (POWERSAVE_PT > 1)
          disable_PT(); //  low, standard, high, we power down at the end of program
        if (POWERSAVE_AUX > 1)
          disable_AUX(); // low, standard, high, we power down at the end of program
        delay(2000);
        progstep = 290;
        first_time = 1;
      }
      // updateMotorVelocities();  //uncomment this for DF Loop
      NunChuckQuerywithEC();
      NunChuckjoybuttons();
      // Check_Prog(); //look for button presses
      //  if (CZ_Button_Read_Count>10 && intval==EXTTRIG_INTVAL ) Interrupt_Fire_Engaged=true; // manual trigger
      //  if (PAUSE_ENABLED && CZ_Button_Read_Count>20 && intval>3 && !Shot_Sequence_Engaged ) Pause_Prog(); //pause an SMS program

      break; // break 250

    case 290: //  finished up pano
      if (first_time == 1)
      {
        lcd.empty();
        stopISR1();
        draw(58, 1, 1); // lcd.at(1,1,"Program Complete");
        draw(59, 2, 1); // lcd.at(2,1," Repeat Press C");
        NunChuckQuerywithEC();
        first_time = 0;
        delay(100);
      }
      NunChuckQuerywithEC();
      NunChuckjoybuttons();
      button_actions290(); // read buttons, look for c button press to start run
      break;               // break 90

    } // switch
  } // while
} // loop