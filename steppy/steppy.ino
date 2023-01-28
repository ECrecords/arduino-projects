#include "steppy.h"

void setup()
{
    pinMode(INPUT1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT1), handle_input1, FALLING);

    pinMode(INPUT2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT2), handle_input2, FALLING);
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
    case STOPPED:
        if (INPUT1)
        {
        }
        else if (INPUT2)
        {
        }
        else
            state = state;
        break;

    case RUNNING:
        if (INPUT1)
        {
        }
        else if (INPUT2)
        {
        }
        else
            state = state;
        break;
    }
}

STEP_STATES_T motor_state = STOPPED;
void loop()
{
    update_state(motor_state);
}