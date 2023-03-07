/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 21:26:50
 * @ Modified by: Your nameno-Islas
 * @ Modified time: 2023-03-06 17:17:20
 * @ Description:
 */
#pragma once

#include <pins_arduino.h>
#include "a4988.h"

#define BAUD_RATE   115200

#define INPUT1      PIN3
#define INPUT2      PIN2

#define STEP        PIN5 
#define ENABLE      PIN6
#define DIR         PIN4

#define SPEED0_RPM   10
#define SPEED1_RPM   30
#define SPEED2_RPM   50
#define SPEED3_RPM   60
#define SPEED4_RPM   70

enum state_codes {STOP_INIT = 0, RUN_INIT, STOP_SP1, RUN_SP1, STOP_SP2, RUN_SP2, STOP_SP3, RUN_SP3, STOP_SP4, RUN_SP4};
enum ret_code {IN1 = 0, IN2, NONE};

extern ret_code (*state[])(void);

#define N_STATES 10
#define N_TRANSITIONS_PER_STATE 3

extern state_codes state_transitions[N_STATES][N_TRANSITIONS_PER_STATE];