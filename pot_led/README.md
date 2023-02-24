# Warm Up Project 1

## Submission Details
**Author:** Elvis Chino-Islas  
**Course:** COMP522  
**Course Name:** Embedded System Applications  
**Completion Date:** 22 February 2023

# Potentiometer LED Program

This is an Arduino program that takes input from a potentiometer and maps the value read from it to a duty cycle for a PWM output. The output is connected to an external LED, and the program includes a calibration process to ensure accurate mapping of the potentiometer output to the desired range of duty cycle values.

## Implementation Details

The program is written in C++ and uses the Arduino IDE. The main program is contained in the `pot_led.cpp` file, which includes the following functions:

- `setup()`: initializes the program by turning on the built-in LED, setting the pins for the external LED and potentiometer, setting the analog reference to 3.3V, enabling UART communication, and calibrating the potentiometer.
- `loop()`: reads the voltage level from the potentiometer, maps the value to a PWM duty cycle, outputs the PWM signal with the determined duty cycle, and sends a message via UART with the current potentiometer and duty cycle values.

The `pot_led.h` file contains some constant definitions, including the pins used for the built-in and external LEDs, the potentiometer pin, and the minimum and maximum duty cycle values for the PWM output.

## Calibration

During the setup phase, the program enters a calibration mode for a specified period of time. To calibrate the potentiometer, swivel it between the maximum and minimum values to ensure accurate calibration. During this period, the program reads values from the potentiometer and determines the minimum and maximum values, which are used to map the potentiometer output to the desired range of duty cycle values. This calibration process is necessary to ensure accurate and consistent behavior of the program, as the range of values output by the potentiometer may vary between different devices and configurations.

## How to Use

To use this program, simply load it onto an Arduino board using the Arduino IDE and connect a potentiometer and LED to the appropriate pins. The program will automatically calibrate the potentiometer during setup and begin outputting PWM signals to the LED based on the potentiometer input. You can monitor the current potentiometer and duty cycle values using a serial monitor connected to the board via UART.

# Video Demo

[Youtube Video Link](https://youtu.be/OhkZhodVj7A)