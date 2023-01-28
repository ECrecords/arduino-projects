/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 21:26:50
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-27 21:36:56
 * @ Description:
 */

#include <pins_arduino.h>

#define INPUT1  PIN2
#define INPUT2  PIN3


enum STEP_STATES_T
{
    STOPPED,
    RUNNING
};