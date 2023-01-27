#pragma once

#include <pins_arduino.h>

#define FREQ_BTN    PIN2

#define T_2HZ_MS    1000 >> 1
#define T_4HZ_MS    1000 >> 2
#define T_8HZ_MS    1000 >> 3
#define T_1HZ_MS    1000

enum state_t
{
    s0_2hz = 0xA,
    s1_4hz,
    s2_8hz,
    s3_1hz
};