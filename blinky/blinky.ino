/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 14:31:21
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-27 18:30:32
 * @ Description:
 *  Program implements a state machine that will
 *  change the frequency of blinking for the on-board
 *  LED.
 */

#include "blinky.h"

// signal used to enable auxilary debug pins to be connected
// to oscilloscope
// #define DEBUG

// Global variables
STATE_T led_state; // holds the current led state

void setup()
{
    // Begins UART with baud of 115200
    Serial.begin(BAUD_RATE);

    // Set on-board LED pin to output
    pinMode(LED_BUILTIN, OUTPUT);

// auxilary signal tracking the on-board
// LED used fro debugging
#ifdef DEBUG
    pinMode(PIN4, OUTPUT);
#endif
    // set pin connected to button as input
    // with a pull-up resistor configured
    pinMode(FREQ_BTN, INPUT_PULLUP);

    // attaches an interrupt onto the button to trigger
    // when a falling edge is detected
    attachInterrupt(digitalPinToInterrupt(FREQ_BTN), handle_btn, FALLING);

    // set the inital state to S0 which will blink the LED
    // at 2 Hz
    led_state = S0_2HZ;
}

// specifies to the compiler that this variable
// can be changed abruptly by an external source
volatile bool interrupt = false;

// ISR to handle a button press
void handle_btn()
{
    // critical region, no interrupts are
    // allowed until inter. is handled
    noInterrupts();
    interrupt = true;
}

// function that handles the transition from state-to-state
void update_state(STATE_T &state)
{
    

    switch (led_state)
    {
    case S0_2HZ:
        led_state = S1_4HZ;
        break;
    case S1_4HZ:
        led_state = S2_8HZ;
        break;
    case S2_8HZ:
        led_state = S3_1HZ;
        break;
    case S3_1HZ:
        led_state = S0_2HZ;
        break;
    }

}

// function uses the supplied state to return
// the period need to blink the LED at a specific frequency
uint32_t get_period(STATE_T state)
{
    switch (state)
    {
    case S0_2HZ:
        return T_2HZ_MS;
    case S1_4HZ:
        return T_4HZ_MS;
    case S2_8HZ:
        return T_8HZ_MS;
    case S3_1HZ:
        return T_1HZ_MS;
    }
}

void loop()
{
    if (interrupt)
    {
        // if button was pressed update the
        // state
        update_state(led_state);

        // delay for 100 ms to add
        // a buffer between button presses
        delay(100);

        // mark button press as handled
        interrupt = false;

        // enable interrupts
        interrupts();

    }

    // get the required period to blink the LED
    // at a specific freq. determined by the current state
    uint32_t t_ms = get_period(led_state);

    // turn on the on-board LED 
    digitalWrite(LED_BUILTIN, HIGH);

#ifdef DEBUG
    digitalWrite(PIN4, HIGH);
#endif

    // hold the LED on for half the period
    delay(t_ms >> 1);

    // turn off the on-board LED
    digitalWrite(LED_BUILTIN, LOW);

#ifdef DEBUG
    digitalWrite(PIN4, LOW);
#endif

    // hold the LED off for half the period
    delay(t_ms >> 1);

}