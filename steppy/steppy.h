/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 21:26:50
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-30 22:41:01
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

enum state_codes {stop_init = 0, run_init, stop_sp1, run_sp1, stop_sp2, run_sp2, stop_sp3, run_sp3, stop_sp4, run_sp4};
enum ret_code {input1 = 0, input2, repeat};

extern ret_code (*state[])(void);

#define N_STATES 10
#define N_TRANSITIONS_PER_STATE 3

extern state_codes state_transitions[N_STATES][N_TRANSITIONS_PER_STATE];