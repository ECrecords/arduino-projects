#include "steppy.h"
#include "a4988.h"


A4988 hw(PIN2, PIN3, PIN4);

void setup()
{
    Serial.begin(BAUD_RATE);
    pinMode(PIN2, OUTPUT);
    pinMode(PIN3, OUTPUT);
    pinMode(PIN4, OUTPUT);
    pinMode(INPUT1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT1), handle_input1, FALLING);

    pinMode(INPUT2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INPUT2), handle_input2, FALLING);

    hw.set_speed(8000);
    hw.set_direction(true);
    hw.start();
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
        Serial.println("STOPPED");
        if (input1_intr)
        {
            input1_intr = false;
            Serial.println("\tINPUT1");
        }
        else if (input2_intr)
        {
            input2_intr = false;
            Serial.println("\tINPUT2");
        }
        else
            state = state;
        break;

    case RUN_INIT:
        Serial.println("RUNNING");
        if (input1_intr)
        {
            input1_intr = false;
            Serial.println("\tINPUT1");
        }
        else if (input2_intr)
        {
            input2_intr = false;
            Serial.println("\tINPUT2");
        }
        else
            state = state;
        break;
    }
}

STEP_STATES_T motor_state = STOPPED;


void loop()
{
    hw.sstep();
    //update_state(motor_state);

//    delay(2000);
}