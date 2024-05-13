#ifndef TBDF_H
#define TBDF_H

#include "TB3_Motor_Control.h"

void DFSetup();
ISR(TIMER1_OVF_vect);
void DFloop();
void updateMotorVelocities();
void setPulsesPerSecond(int motorIndex, uint16_t pulsesPerSecond);
void setupMotorMove(int motorIndex, int32_t destination);
void hardStop();
void stopMotor(int motorIndex);
boolean isValidMotor(int motorIndex);
void processGoPosition(int motorIndex, int32_t pos);
byte processUserMessage(char data);
void processSerialCommand();
void sendMessage(byte msg, byte motorIndex);
void nextMessage();
boolean jogMotor(int motorIndex, int32_t target, int32_t *destination);
void inchMotor(int motorIndex, int32_t target);
void calculatePointToPoint(int motorIndex, int32_t destination);
void setupBlur(int motorIndex, int exposure, int blur, int32_t p0, int32_t p1, int32_t p2);
float calculateVelocityMotor(int motorIndex, float local_time, float local_ramp);
void calculateVelocityMotorold(int motorIndex, float local_time, float local_ramp);
void synched3PtMove_max(float xtarget, float ytarget, float ztarget);
void synched3AxisMove_timed(float xtarget, float ytarget, float ztarget, float local_time, float local_ramp);
void calculatePointToPoint_move(int motorIndex);
void calculatePointToPoint_jog(int motorIndex, int32_t destination);

#endif

#define SERIAL_DEVICE Serial

#define PIN_ON(port, pin) \
  {                       \
    port |= pin;          \
  }
#define PIN_OFF(port, pin) \
  {                        \
    port &= ~pin;          \
  }

extern byte motorMoving;
extern uint16_t *motorMoveSpeed[];
extern uint16_t *motorMoveSteps[];
extern boolean maxVelLimit;
extern const uint8_t MOTOR_COUNT;
extern const uint16_t PAN_MAX_JOG_STEPS_PER_SEC;
extern const uint16_t TILT_MAX_JOG_STEPS_PER_SEC;
extern uint16_t AUX_MAX_JOG_STEPS_PER_SEC;
extern const uint8_t MOTOR0_STEP;
extern const uint8_t MOTOR1_STEP;
extern const uint8_t MOTOR2_STEP;
extern const uint8_t MOTOR0_DIR;
extern const uint8_t MOTOR1_DIR;
extern const uint8_t MOTOR2_DIR;
extern const uint8_t MOTOR_COUNT;
extern uint16_t *motorAccumulator[];

extern uint16_t motorMoveSteps0;
extern uint16_t motorMoveSteps1;
extern uint16_t motorMoveSteps2;
extern uint16_t motorMoveSteps3;
extern uint16_t motorAccumulator0;
extern uint16_t motorAccumulator1;
extern uint16_t motorAccumulator2;
extern uint16_t motorAccumulator3;
extern uint16_t motorMoveSpeed0;
extern uint16_t motorMoveSpeed1;
extern uint16_t motorMoveSpeed2;
extern uint16_t motorMoveSpeed3;
extern uint8_t MOTOR0_STEP_PORT;
extern const uint8_t MOTOR0_STEP_PIN;

extern uint8_t MOTOR1_STEP_PORT;
extern const uint8_t MOTOR1_STEP_PIN;

extern uint8_t MOTOR2_STEP_PORT;
extern const uint8_t MOTOR2_STEP_PIN;

extern uint8_t MOTOR3_STEP_PORT;
extern const uint8_t MOTOR3_STEP_PIN;