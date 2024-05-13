#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <Arduino.h>

bool camera_shutter();

void setup_camera();

void focus_camera();
void fire_camera();
void fire_camera(unsigned long exp_tm);
void stop_camera();

void MsTimer2_set(unsigned long ms);
void MsTimer2_start();
void MsTimer2_overflow();

void MsTimer3_stop();

#endif