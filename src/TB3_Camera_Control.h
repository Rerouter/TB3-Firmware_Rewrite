#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <Arduino.h>

void setup_camera();

bool camera_shutter();
bool camera_focus();

void focus_camera();
void fire_camera();
void fire_camera(unsigned long exp_tm);
void stop_camera();

void MsTimer2_set(unsigned long ms);
void MsTimer2_start();
void MsTimer2_overflow();
void MsTimer2_stop();

#endif