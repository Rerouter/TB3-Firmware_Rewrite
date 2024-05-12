#include <Arduino.h>
#include <EEPROM.h>

extern int sequence_repeat_type;
extern unsigned int first_time;
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

extern unsigned int POWERSAVE_PT;
extern unsigned int POWERSAVE_AUX;
extern byte PAUSE_ENABLED;
extern unsigned int LCD_BRIGHTNESS_DURING_RUN;
extern uint16_t AUX_MAX_JOG_STEPS_PER_SEC;
extern byte AUX_ON;
extern byte AUX_REV;
extern unsigned long build_version;

extern unsigned int progstep;
extern unsigned int progtype;
extern boolean Program_Engaged;

unsigned long check_version();
boolean eeprom_saved();
void eeprom_saved(boolean saved);

void eeprom_write_final(int pos, byte &val, byte len);
void eeprom_write(int pos, unsigned int &val);
void eeprom_write(int pos, unsigned long &val);
void eeprom_write(int pos, float &val);
void eeprom_write(int pos, byte &val);

void eeprom_read(int pos, byte &val, byte len);
void eeprom_read(int pos, byte &val);
void eeprom_read(int pos, int &val);
void eeprom_read(int pos, unsigned int &val);
void eeprom_read(int pos, unsigned long &val);
void eeprom_read(int pos, float &val);

void set_defaults_in_ram();
void set_defaults_in_setup();
void write_defaults_to_eeprom_memory();
void write_all_ram_to_eeprom();
void restore_from_eeprom_memory();

void review_RAM_Contents();
