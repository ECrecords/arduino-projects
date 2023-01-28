/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 21:26:50
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-28 14:51:10
 * @ Description:
 */
#pragma once

#include <pins_arduino.h>
#include "a4988.h"

#define BAUD_RATE   115200

#define INPUT1      PIN2
#define INPUT2      PIN3

#define STEP        PIN5 
#define ENABLE      PIN4
#define DIR         PIN6
enum STEP_STATES_T
{
    STOP_INIT,
    RUN_INIT,

    RUN_SPEED1,
    STOP_SPEED1,
    
    RUN_SPEED2,
    STOP_SPEED2,
    
    RUN_SPEED3,
    STOP_SPEED3,

    RUN_SPEED4,
    STOP_SPEED4,
};

#define SPEED1_RPM   6000
#define SPEED2_RPM   8000
#define SPEED3_RPM   12000
#define SPEED4_RPM   16000