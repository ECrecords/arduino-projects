#include "a4988.h"

A4988::A4988(uint8_t ena_pin, uint8_t step_pin, uint8_t dir_pin)
    : enable(ena_pin), step(step_pin), dir(dir_pin), t_ms(0)
{
    pinMode(enable, OUTPUT);
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
}

A4988::A4988(){}

A4988::~A4988()
{
}

void A4988::init(uint32_t rpm, bool clockwise)
{
    set_speed(rpm);
    set_direction(clockwise);
    start();
}

void A4988::set_speed(uint32_t rpm)
{
    this->rpm = rpm;

    float freq = (rpm/60.0)*200.0;

    t_ms = static_cast<uint32_t>((1/freq * 1000));
}

void A4988::set_direction(bool clockwise)
{
    digitalWrite(dir, (clockwise) ? HIGH : LOW);
}
void A4988::start()
{
    digitalWrite(enable, LOW);
}

void A4988::stop()
{
    digitalWrite(enable, HIGH);
}

void A4988::sstep()
{
    digitalWrite(step, HIGH);
    delay(static_cast<int>(t_ms) >> 1);

    digitalWrite(step, LOW);
    delay(t_ms - (static_cast<int>(t_ms) >> 1));
}