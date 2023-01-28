#pragma once

#include <Arduino.h>

typedef uint8_t pin_t;

class A4988
{
private:
    pin_t enable;
    pin_t step;
    pin_t dir;

public:
    float t_ms;

public:
    A4988(uint8_t ena_pin, uint8_t step_pin, uint8_t dir_pin);
    A4988();
    ~A4988();

    void init(uint32_t rpm, bool clockwise);

    void set_direction(bool clockwise);

    void set_speed(uint32_t rpm);

    void start();

    void stop();

    void sstep();
};
