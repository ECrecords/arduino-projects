/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 23:07:07
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-30 22:31:38
 * @ Description:
 */

#include "steppy.h"

A4988 hw;
state_codes curr_state = stop_init;

ret_code (*state[])(void) =
    {
        stopped,
        update_speed};

state_codes state_transitions[N_STATES][N_TRANSITIONS_PER_STATE] = {

    {run_init, stop_init, stop_init},
    {run_sp1, stop_sp1, run_init},

    {run_sp1, run_init, stop_sp1},
    {run_sp2, stop_sp1, run_sp1},

    {run_sp2, run_init, stop_sp2},
    {run_sp3, stop_sp2, run_sp2},

    {run_sp3, run_init, stop_sp3},
    {run_sp4, stop_sp3, run_sp3},

    {run_sp4, run_init, stop_sp4},
    {run_sp4, stop_sp4, run_sp4}};

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
        return input1;
    }
    else if (input2_intr)
    {
        delay(100);
        input2_intr = false;
        return input2;
    }
    else
        return repeat;
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
    case run_init:
        rpm = SPEED0_RPM;
        break;
    case run_sp1:
        rpm = SPEED1_RPM;
        break;
    case run_sp2:
        rpm = SPEED2_RPM;
        break;
    case run_sp3:
        rpm = SPEED3_RPM;
        break;
    case run_sp4:
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

void loop()
{
    auto state_action = state[curr_state % 2];
    ret_code rc = state_action();
    curr_state = state_transitions[curr_state][rc];

    hw.sstep();

    char buff[512];

    sprintf(buff, "Current State : %2i | currAngVelocity: %3i RPM \r\n", curr_state, hw.rpm);
    Serial.print(buff);
}