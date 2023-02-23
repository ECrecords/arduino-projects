/**
 * @ Author: Elvis Chino-Islas
 * @ Create Time: 2023-01-27 14:31:21
 * @ Modified by: Elvis Chino-Islas
 * @ Modified time: 2023-02-22 04:37:33
 * @ Description:
 *  Program takes input from a potentiometer
 *  and maps the value read from it to duty cycle
 *  to a PWM output which is connected to an external LED.
 */
#include "pot_led.h"

uint32_t min_pot_val = UINT10_MAX;
uint32_t max_pot_val = 0;

void setup()
{

    // Turn on on-board LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Set PIN3 to output which is connected
    // to external LED
    pinMode(EXTERNAL_LED_PIN, OUTPUT);

    // Set analog ref. to 3v3
    analogReference(DEFAULT);

    // Enable UART with baud of 115200
    Serial.begin(BAUD_RATE);

    // calibrate potentiometer
    uint32_t pot2calibrate;

    // print calibration message
    Serial.println(CALIBRATION_MESSAGE);

    // enter calibration mode for specified period
    while (millis() < CALIBRATION_MODE_PERIOD_MS)
    {
        pot2calibrate = analogRead(POTENTIOMETER_PIN);

        if (pot2calibrate > max_pot_val)
            max_pot_val = pot2calibrate;

        if (pot2calibrate < min_pot_val)
            min_pot_val = pot2calibrate;
    }
}

void loop()
{
    // declare buffer to hold message to be send via UART
    char buff[BUFFER_SZ];

    // read voltage level from potentiometer
    int pot_val = analogRead(POTENTIOMETER_PIN);

    // map potentiometer value to PWM duty cycle (0-255)
    long duty = map(pot_val, min_pot_val, max_pot_val, MIN_DUTY, MAX_DUTY);

    // output PWM signal with determined duty cycle
    analogWrite(EXTERNAL_LED_PIN, static_cast<int>(duty));

    // uses a format specifier to build the message to be sent
    snprintf(buff, BUFFER_SZ, "Pot Val: %4i, Duty: %3ld ", pot_val, duty);

    // send current pot. and duty val
    Serial.println(buff);
}