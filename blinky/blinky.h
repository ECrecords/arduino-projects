#pragma once

#include <pins_arduino.h>

#define BAUD_RATE   115200
#define FREQ_BTN    PIN2

#define T_2HZ_MS    1000 >> 1
#define T_4HZ_MS    1000 >> 2
#define T_8HZ_MS    1000 >> 3
#define T_1HZ_MS    1000

enum STATE_T
{
    S0_2HZ = 0xA,
    S1_4HZ,
    S2_8HZ,
    S3_1HZ
};