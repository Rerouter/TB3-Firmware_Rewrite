#ifndef TB3EEPROM_H
#define TB3EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Structs.h"

unsigned int check_version();
boolean eeprom_saved();
void eeprom_saved(boolean saved);

// Template function for writing data of any type to EEPROM
template<typename T>
void eeprom_write(int pos, const T& val);

// Template function for reading data of any type from EEPROM
template<typename T>
void eeprom_read(int pos, T& val);

void set_defaults_in_ram();
void set_defaults_in_setup();
void write_defaults_to_eeprom_memory();
void write_all_ram_to_eeprom();
void restore_from_eeprom_memory();

void review_RAM_Contents();

#endif


extern int sequence_repeat_type;
extern boolean first_time;
extern unsigned int intval;
extern unsigned long interval;
extern unsigned int camera_fired;
extern unsigned int camera_moving_shots;
extern unsigned int camera_total_shots;
extern unsigned int overaldur;
extern unsigned int prefire_time;
extern unsigned int rampval;
extern unsigned int static_tm;
extern unsigned int lead_in;
extern unsigned int lead_out;
extern float motor_steps_pt[3][3];
extern unsigned int keyframe[2][6];
extern float linear_steps_per_shot[3];
extern float ramp_params_steps[3];

extern PowerSave POWERSAVE_PT;
extern PowerSave POWERSAVE_AUX;
extern boolean PAUSE_ENABLED;
extern uint8_t LCD_BRIGHTNESS_DURING_RUN;
extern uint16_t AUX_MAX_JOG_STEPS_PER_SEC;
extern boolean AUX_ON;
extern boolean AUX_REV;
extern unsigned int build_version;

extern unsigned int progstep;
extern int8_t progtype;
extern boolean Program_Engaged;
extern FloatPoint current_steps;