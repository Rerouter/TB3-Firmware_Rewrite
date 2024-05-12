#include <Arduino.h>

#ifndef STRUCTS_H
#define STRUCTS_H

struct FloatPoint
{
  float x;
  float y;
  float z;
};

#define USER_CMD_ARGS 40

struct UserCmd
{
  byte command;
  byte argCount;
  int32_t args[USER_CMD_ARGS];
};

struct txMsg
{
  byte msg;
  byte motor;
};

#define TX_MSG_BUF_SIZE 16

struct TxMsgBuffer
{
  txMsg buffer[TX_MSG_BUF_SIZE];
  byte head;
  byte tail;
};

#define P2P_MOVE_COUNT 7

struct Motor
{
  byte stepPin;
  byte dirPin;

  // pre-computed move
  float moveTime[P2P_MOVE_COUNT];
  int32_t movePosition[P2P_MOVE_COUNT];
  float moveVelocity[P2P_MOVE_COUNT];
  float moveAcceleration[P2P_MOVE_COUNT];

  float gomoMoveTime[P2P_MOVE_COUNT];
  int32_t gomoMovePosition[P2P_MOVE_COUNT];
  float gomoMoveVelocity[P2P_MOVE_COUNT];
  float gomoMoveAcceleration[P2P_MOVE_COUNT];

  int currentMove;
  float currentMoveTime;

  volatile boolean dir;

  int32_t position;
  int32_t destination;

  float maxVelocity;     // Orig - delete later
  float maxAcceleration; // Orig - delete later

  float moveMaxVelocity;     // Pass this into calculator for synchronized moves
  float moveMaxAcceleration; // Pass this into calculator for synchronized moves

  float jogMaxVelocity;     // replaced the original maxVelocity
  float jogMaxAcceleration; // replaced the original maxAcceleration

  uint16_t nextMotorMoveSteps;
  float nextMotorMoveSpeed;
};

#endif