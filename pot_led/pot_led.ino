/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 14:31:21
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-01-27 17:43:22
 * @ Description: 
 *  Program takes input from a potentiometer
 *  and maps the value read from it to duty cycle
 *  to a PWM output which is connected to an external LED.
 */

#include <pins_arduino.h>
#include "pot_led.h"

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
    uint32_t pot_val = analogRead(A0);
    uint32_t duty = map(pot_val, 0, UINT10_MAX, MIN_DUTY_MS, MAX_DUTY_MS);

    if (duty == MIN_DUTY_MS)
        digitalWrite(PIN3, LOW);
    else if (duty > MAX_DUTY_MS - 2)
        digitalWrite(PIN3, HIGH);
    else
    {
        digitalWrite(PIN3, LOW);
        delay(T_50HZ_MS - duty);
        digitalWrite(PIN3, HIGH);
        delay(duty);
    }
}