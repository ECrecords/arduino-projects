/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 23:07:07
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-29 09:09:01
 * @ Description:
 */

#include "steppy.h"

A4988 hw;

void setup()
{
    Serial.begin(BAUD_RATE);
    pinMode(INPUT1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT1), handle_input1, FALLING);

    pinMode(INPUT2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT2), handle_input2, FALLING);

    hw = A4988(ENABLE, STEP, DIR);

    hw.init(8000, true);
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

ret_code run_init_state(void)
{
    hw.set_speed(2000);
    hw.start();

    return det_state();
}
ret_code run_sp1_state(void)
{
    hw.set_speed(SPEED1_RPM);
    hw.start();

    return det_state();
}
ret_code run_sp2_state(void)
{
    hw.set_speed(SPEED2_RPM);
    hw.start();

    return det_state();
}

ret_code run_sp3_state(void)
{
    hw.set_speed(SPEED3_RPM);
    hw.start();

    return det_state();
}

ret_code run_sp4_state(void)
{
    hw.set_speed(SPEED4_RPM);
    hw.start();

    return det_state();
}

ret_code stopped(void)
{
    hw.stop();

    return det_state();
}

ret_code (*state[])(void) =
    {
        stopped,
        run_init_state,

        stopped,
        run_sp1_state,

        stopped,
        run_sp2_state,

        stopped,
        run_sp3_state,

        stopped,
        run_sp4_state};

state_codes state_transitions[N_STATES][N_TRANSITIONS_PER_STATE] = {

        [stop_init] = {run_sp1, stop_init, run_init},
        [run_init]  = {run_init, stop_init, stop_init},

        [stop_init] ={run_sp1, run_init, stop_sp1},
        [run_init]  ={run_sp2, stop_sp1, run_sp1},

        [stop_init] ={run_sp2, run_init, stop_sp2},
        [run_init]  ={run_sp3, stop_sp2, run_sp2},

        [stop_init] ={run_sp3, run_init, stop_sp3},
        [run_init]  ={run_sp4, stop_sp3, run_sp3},
        
        [stop_init] ={run_sp4, run_init, stop_sp4},
        [run_init]  ={run_sp4, stop_sp4, run_sp4}
};

state_codes curr_state;

void loop()
{
    auto state_action = state[curr_state];
    ret_code rc = state_action();
    curr_state = state_transitions[curr_state][rc];

    hw.sstep();
    char buff[255];
    state_codes curr_state_cpy = curr_state;
    sprintf(buff, "Current State : %2i | Return Code : %2i\r", curr_state_cpy, rc);
    Serial.print(buff);
}