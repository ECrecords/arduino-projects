#include <Arduino.h>

class ShiftRegister
{
private:
    uint8_t __clockPin;
    uint8_t __latchPin;
    uint8_t __dataPin;

    uint8_t __data;

public:
    ShiftRegister(uint8_t clockPin, uint8_t latchPin, uint8_t dataPin); 

    uint8_t get_data();

    void write_to_pin(uint8_t pin, bool value);

    void write_data(uint8_t value);

    bool get_pin(uint8_t pin);
};