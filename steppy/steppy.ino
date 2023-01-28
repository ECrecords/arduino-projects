/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 23:07:07
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-28 15:05:53
 * @ Description:
 */

#include "steppy.h"

A4988 hw;
STEP_STATES_T motor_state;
void setup()
{
    Serial.begin(BAUD_RATE);
    pinMode(INPUT1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT1), handle_input1, FALLING);

    pinMode(INPUT2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT2), handle_input2, FALLING);

    hw = A4988(ENABLE, STEP, DIR);

    hw.init(8000, true);
    motor_state = STOP_INIT;
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

void update_state(STEP_STATES_T &state)
{
    switch (state)
    {
    case STOP_INIT:
        if (input1_intr)
        {
            state = RUN_INIT;
            input1_intr = false;
        }
        else if (input2_intr)
        {
            input2_intr = false;
        }
        else
            state = state;
        break;

    case RUN_INIT:
        if (input1_intr)
        {
            state = RUN_SPEED1;
            input1_intr = false;
        }
        else if (input2_intr)
        {
            state = STOP_INIT;
            input2_intr = false;
        }
        else
            state = state;
        break;
        
    case RUN_SPEED1:
        if (input1_intr)
        {
            state = RUN_SPEED2;
            input1_intr = false;
        }
        else if (input2_intr)
        {
            state = STOP_SPEED1;
            input2_intr = false;
        }
        else
            state = state;
        break;

    case RUN_SPEED2:
        if (input1_intr)
        {
            state = RUN_SPEED3;
            input1_intr = false;
        }
        else if (input2_intr)
        {
            state = STOP_SPEED2;
            input2_intr = false;
        }
        else
            state = state;
        break;

    case RUN_SPEED3:
        if (input1_intr)
        {
            state = RUN_SPEED4;
            input1_intr = false;
        }
        else if (input2_intr)
        {
            state = STOP_SPEED3;
            input2_intr = false;
        }
        else
            state = state;
        break;
        
    case RUN_SPEED4:
        if (input1_intr)
        {
            input1_intr = false;
        }
        else if (input2_intr)
        {
            state = STOP_SPEED3;
            input2_intr = false;
        }
        else
            state = state;
        break;
    }
}

inline void rpm_start(A4988 driver, uint32_t rpm)
{
    driver.set_speed(rpm);
    driver.stop();
}

void state_action(STEP_STATES_T state)
{
    switch (state)
    {
    case STOP_INIT:
        Serial.println("STOPPED");
        hw.stop();
        break;
    case RUN_INIT:
        Serial.println("INIT");
        hw.start();
        break;

    case RUN_SPEED1:
        rpm_start(hw, SPEED1_RPM);
        break;
    case STOP_SPEED1:
        hw.stop();
        break;

    case RUN_SPEED2:
        rpm_start(hw, SPEED2_RPM);
        break;
    case STOP_SPEED2:
        hw.stop();
        break;

    case RUN_SPEED3:
        rpm_start(hw, SPEED3_RPM);
        break;
    case STOP_SPEED3:
        hw.stop();
        break;

    case RUN_SPEED4:
        rpm_start(hw, SPEED4_RPM);
        break;
    case STOP_SPEED4:
        hw.stop();
        break;
    }
}

void loop()
{
    update_state(motor_state);
    state_action(motor_state);

    hw.sstep();
}