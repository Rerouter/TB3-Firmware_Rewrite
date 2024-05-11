#include "TB3_IO_ISR.h"

const uint8_t IO_2 = 2;               // drives middle of 2.5 mm connector on I/O port
const uint8_t IO_3 = 3;                // drives tip of 2.5 mm connector on I/O port

void setup_IO() {
    // Setup of I/0 Pings Start with output of I/Oport
  pinMode(IO_2, OUTPUT);
  pinMode(IO_3, OUTPUT);

  digitalWrite(IO_2, LOW);
  digitalWrite(IO_3, LOW);

  init_external_triggering();
}


void init_external_triggering()
{
  pinMode(IO_3, INPUT);
  digitalWrite(IO_3, HIGH);
  attachInterrupt(1, cam_change, CHANGE);
}

void cam_change()
{
  changehappened = true;
  state = digitalRead(3);
  if (DEBUG)
    Serial.print("i");
}

void setupstartISR1()
{
  TCCR1A = 0;
  TCCR1B = _BV(WGM13);

  ICR1 = (F_CPU / 4000000) * TIME_CHUNK; // goes twice as often as time chunk, but every other event turns off pins
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TIMSK1 = _BV(TOIE1);
  TCCR1B |= _BV(CS10);
}

void startISR1()
{

  TIMSK1 = _BV(TOIE1);
}

void stopISR1()
{
  TIMSK1 &= ~_BV(TOIE1);
}

void Jogloop()
{
  while (true) // short fast loop pull this out later
  {
    if (!nextMoveLoaded)
      updateMotorVelocities2();
  }
} // end of loop
