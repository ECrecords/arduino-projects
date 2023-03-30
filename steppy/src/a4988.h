#pragma once

#include <Arduino.h>
#include <pins_arduino.h>

typedef uint8_t pin_t;

/// @brief driver class used to operate a A4988 stepper motor driver
class A4988
{
private:
    pin_t enable;
    pin_t step;
    pin_t dir;

    pin_t ms1;
    pin_t ms2;
    pin_t ms3;

public:
  uint32_t t_ms;
  uint32_t rpm;


public:
    /// @brief create an instance of a A4988 stepper motor driver
    /// @param ena_pin pin connected to the ENABLE pin 
    /// @param step_pin pin connected to the STEP pin
    /// @param dir_pin pin connected tot he DIR pin
    A4988(uint8_t ena_pin, uint8_t step_pin, uint8_t dir_pin);

    /// @brief used to create an uinitialized instance
    A4988();

    
    ~A4988();

    /// @brief 
    /// @param rpm 
    /// @param clockwise 
    void init(uint32_t rpm, bool clockwise);

    /// @brief set the direction of roation for the stepper motor 
    /// @param clockwise if true, motor spins in a clockwise direction, otherwise, it spins counter-clockwise
    void set_direction(bool clockwise);

    /// @brief set the speed of rotation for the stepper motor in rotations per minute (RPM) units
    /// @param rpm  speed of rotation to set in RPM
    void set_speed(uint32_t rpm);

    /// @brief set the ENABLE pin to LOW, enabling the A4988 driver 
    void start();

    /// @brief set the ENABLE pin to HIGH, disabling the A4988 driver
    void stop();

    /// @brief send a single PWM pulse through STEP pin 
    void sstep();
};
