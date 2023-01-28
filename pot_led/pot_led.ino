/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 14:31:21
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-27 18:03:13
 * @ Description: 
 *  Program takes input from a potentiometer
 *  and maps the value read from it to duty cycle
 *  to a PWM output which is connected to an external LED.
 */

#include <pins_arduino.h>

// In-line constants
#define MAX_DUTY    255   
#define MIN_DUTY    0
#define UINT10_MAX  1023
#define BAUD_RATE   115200

void setup()
{
    // Turn on on-board LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Set PIN3 to output which is connected
    // to external LED
    pinMode(PIN3, OUTPUT);

    // Set analog ref. to 3v3
    analogReference(DEFAULT);
    
    // Enable UART with baud of 115200
    Serial.begin(BAUD_RATE);
}

void loop()
{
    // read voltage level from potentiometer
    uint32_t pot_val = analogRead(A0);

    // map potentiometer value (0-1023) to PWM duty cycle (0-255)
    uint32_t duty = map(pot_val, 0, UINT10_MAX, MIN_DUTY, MAX_DUTY);

    // output PWM signal with determined duty cycle
    analogWrite(PIN3, duty);
}