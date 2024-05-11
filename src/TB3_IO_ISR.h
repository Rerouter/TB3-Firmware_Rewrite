#include <Arduino.h>
#include <avr/interrupt.h>

extern volatile boolean changehappened;
extern volatile boolean nextMoveLoaded;
extern volatile int state;
extern const bool DEBUG;
extern const uint8_t TIME_CHUNK;
extern void updateMotorVelocities2();

void setup_IO();
void init_external_triggering();
void cam_change();
void setupstartISR1();
void startISR1();
void stopISR1();
void Jogloop();

