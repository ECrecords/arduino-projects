#include <shift_register.h>

ShiftRegister::ShiftRegister(){}

ShiftRegister::ShiftRegister(uint8_t clockPin, uint8_t latchPin, uint8_t dataPin): __clockPin(clockPin), __latchPin(latchPin), __dataPin(dataPin) 
{
    pinMode(__clockPin, OUTPUT);
    pinMode(__latchPin, OUTPUT);
    pinMode(__dataPin, OUTPUT);

    digitalWrite(__latchPin, HIGH);

    write_data(uint8_t(0x00));
}

bool ShiftRegister::get_pin(uint8_t pin)
{
    uint8_t mask = 1 << pin;
    return bool(mask & get_data());
}

uint8_t ShiftRegister::get_data()
{
    return __data;
}

void ShiftRegister::write_data(uint8_t value)
{
    digitalWrite(__latchPin, LOW);
    shiftOut(__dataPin, __clockPin, MSBFIRST, value);
    digitalWrite(__latchPin, HIGH);

    __data = value;
}

void ShiftRegister::write_to_pin(uint8_t pin, bool value)
{
    uint8_t pinMask = 1 << pin;
    uint8_t pinValue = uint8_t(value) << pin;
    uint8_t newData =  (__data & ~pinMask) | pinValue;
    write_data(newData);
}

void ShiftRegister::set_mask(uint8_t mask)
{
    write_data(__data | mask);
}

void ShiftRegister::clear_mask(uint8_t mask)
{
    write_data(__data & ~mask);
}