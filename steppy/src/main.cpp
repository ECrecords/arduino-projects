/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 23:07:07
 * @ Modified by: Your name
 * @ Modified time: 2023-03-29 21:58:59
 * @ Description:
 */

#include "steppy.h"

A4988 hw;

/// @brief Holds the current state of the program, its initalized 
/// with state enumerated by STOP_INIT
state_codes curr_state = STOP_INIT;

/// @brief Defines all possible state transitions
/// ROWS represent transtions per state
state_codes state_transitions[N_STATES][N_TRANSITIONS_PER_STATE] = {

    // STOP_INIT
    {RUN_INIT, STOP_INIT, STOP_INIT},
    // RUN_INIT
    {RUN_SP1, STOP_SP1, RUN_INIT},

    // STOP_SP1
    {RUN_SP1, STOP_INIT, STOP_SP1},
    // RUN_SP1
    {RUN_SP2, STOP_SP1, RUN_SP1},

    // STOP_SP2
    {RUN_SP2, STOP_INIT, STOP_SP2},
    // RUN_SP2
    {RUN_SP3, STOP_SP2, RUN_SP2},

    // STOP_SP3
    {RUN_SP3, STOP_INIT, STOP_SP3},
    // RUN_SP3
    {RUN_SP4, STOP_SP3, RUN_SP3},

    // STOP_SP4
    {RUN_SP4, STOP_INIT, STOP_SP4},
    // RUN_SP4
    {RUN_SP4, STOP_SP4, RUN_SP4}};

void setup()
{
    Serial.begin(BAUD_RATE);

    pinMode(INPUT1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT1), handle_input1, FALLING);

    pinMode(INPUT2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT2), handle_input2, FALLING);

    hw = A4988(ENABLE, STEP, DIR);

    hw.init(SPEED1_RPM, true);
}

volatile bool input1_intr = false;
void handle_input1()
{
    input1_intr = true;
}

volatile bool input2_intr = false;
void handle_input2()
{
    input2_intr = true;
}

inline ret_code det_state()
{
    if (input1_intr)
    {
        delay(100);
        input1_intr = false;
        return IN1;
    }
    else if (input2_intr)
    {
        delay(100);
        input2_intr = false;
        return IN2;
    }
    else
        return NONE;
}

ret_code stopped(void)
{
    hw.stop();

    return det_state();
}

ret_code update_speed()
{

    uint32_t rpm;

    switch (curr_state)
    {
    case RUN_INIT:
        rpm = SPEED0_RPM;
        break;
    case RUN_SP1:
        rpm = SPEED1_RPM;
        break;
    case RUN_SP2:
        rpm = SPEED2_RPM;
        break;
    case RUN_SP3:
        rpm = SPEED3_RPM;
        break;
    case RUN_SP4:
        rpm = SPEED4_RPM;
        break;
    default:
        rpm = SPEED0_RPM;
        break;
    }

    hw.set_speed(rpm);
    hw.start();

    return det_state();
}

ret_code (*state[])(void) = {stopped, update_speed};

void loop()
{
    auto state_action = state[curr_state % 2];
    ret_code rc = state_action();
    curr_state = state_transitions[curr_state][rc];
    
    hw.sstep();

    char buff[512];
    sprintf(buff, "Current State : %2i | currAngVelocity: %2lu RPM \r", curr_state, hw.rpm);
    Serial.print(buff);
}