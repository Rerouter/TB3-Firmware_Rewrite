#include "TB3_Camera_Control.h"

// Timer2flags
uint32_t MsTimer2_msecs;
volatile uint32_t MsTimer2_count;
volatile char MsTimer2_overflowing;
volatile uint16_t MsTimer2_tcnt2;

const uint8_t CAMERA_PIN = 12; // drives tip of 2.5 mm connector
const uint8_t FOCUS_PIN = 13;  // drives  middle of 2.5mm connector
boolean Shutter_Signal_Engaged = false;
boolean Focus_Signal_Engaged = false;

/*

  ========================================
  Camera control functions
  ========================================

*/

void setup_camera()
{
  // setup camera pins
  pinMode(CAMERA_PIN, OUTPUT);
  pinMode(FOCUS_PIN, OUTPUT);

  digitalWrite(CAMERA_PIN, LOW);
  digitalWrite(FOCUS_PIN, LOW);
}

bool camera_shutter()
{
  return Shutter_Signal_Engaged;
}

bool camera_focus()
{
  return Focus_Signal_Engaged;
}

void focus_camera()
{
  digitalWrite(FOCUS_PIN, HIGH); // for longer shot interval, wake up the camera
  Focus_Signal_Engaged = true;
}

void fire_camera()
{
  digitalWrite(FOCUS_PIN, HIGH);
  digitalWrite(CAMERA_PIN, HIGH);
  Shutter_Signal_Engaged = true;
  Focus_Signal_Engaged = true;
}

void fire_camera(unsigned long exp_tm)
{
  digitalWrite(FOCUS_PIN, HIGH);
  digitalWrite(CAMERA_PIN, HIGH);

  // start timer to stop camera exposure
  MsTimer2_set(exp_tm);
  MsTimer2_start();

  Shutter_Signal_Engaged = true;
  Focus_Signal_Engaged = true;

  return;
}

void stop_camera()
{
  MsTimer2_stop();
  digitalWrite(CAMERA_PIN, LOW);
  digitalWrite(FOCUS_PIN, LOW);

  Shutter_Signal_Engaged = false;
  Focus_Signal_Engaged = false;
}

//***************************************************************************************//
// Start of MSTimer2 routines - this is
void (*MsTimer2_func)();

void MsTimer2_set(unsigned long ms)
{
  float prescaler = 0.0;

  if (ms == 0)
    MsTimer2_msecs = 1;
  else
    MsTimer2_msecs = ms;

  // func = f;

  TIMSK2 &= ~(1 << TOIE2);
  TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
  TCCR2B &= ~(1 << WGM22);
  ASSR &= ~(1 << AS2);
  TIMSK2 &= ~(1 << OCIE2A);

  TCCR2B |= (1 << CS22);
  TCCR2B &= ~((1 << CS21) | (1 << CS20));
  prescaler = 64.0;

  MsTimer2_tcnt2 = 256 - (int)((float)F_CPU * 0.001 / prescaler);
}

void MsTimer2_start()
{
  MsTimer2_count = 0;
  MsTimer2_overflowing = 0;
  TCNT2 = MsTimer2_tcnt2;
  TIMSK2 |= (1 << TOIE2);
}

void MsTimer2_stop()
{
  TIMSK2 &= ~(1 << TOIE2);
}

void MsTimer2_overflow()
{
  MsTimer2_count += 1;

  if (MsTimer2_count >= MsTimer2_msecs && !MsTimer2_overflowing)
  {
    MsTimer2_overflowing = 1;
    MsTimer2_count = MsTimer2_count - MsTimer2_msecs; // subtract ms to catch missed overflows
                                                      // set to 0 if you don't want this.
    stop_camera();
    MsTimer2_overflowing = 0;
  }
}

ISR(TIMER2_OVF_vect)
{

  TCNT2 = MsTimer2_tcnt2;
  MsTimer2_overflow();
}
