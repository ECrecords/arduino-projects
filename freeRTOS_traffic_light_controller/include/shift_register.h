/**
 * @file shift_register.h
 * @brief Library for controlling a shift register with a microcontroller.
 */

#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include <Arduino.h>

/**
 * @class ShiftRegister
 * @brief Class for controlling a shift register with a microcontroller.
 */
class ShiftRegister
{
private:
    uint8_t __clockPin; /**< Pin number for the clock signal. */
    uint8_t __latchPin; /**< Pin number for the latch signal. */
    uint8_t __dataPin; /**< Pin number for the data signal. */

    uint8_t __data; /**< Current state of the shift register. */

public:

    /**
     * @brief Default constructor for the ShiftRegister class.
     */
    ShiftRegister();

    /**
     * @brief Constructor for the ShiftRegister class.
     * @param clockPin Pin number for the clock signal.
     * @param latchPin Pin number for the latch signal.
     * @param dataPin Pin number for the data signal.
     */
    ShiftRegister(uint8_t clockPin, uint8_t latchPin, uint8_t dataPin); 

    /**
     * @brief Gets the current state of the shift register.
     * @return The current state of the shift register.
     */
    uint8_t get_data();

    /**
     * @brief Writes a boolean value to a specific pin of the shift register.
     * @param pin The pin number to write to.
     * @param value The boolean value to write (true = high, false = low).
     */
    void write_to_pin(uint8_t pin, bool value);

    /**
     * @brief Writes an 8-bit value to the shift register.
     * @param value The value to write.
     */
    void write_data(uint8_t value);

    /**
     * @brief Gets the boolean state of a specific pin of the shift register.
     * @param pin The pin number to read from.
     * @return The boolean state of the specified pin.
     */
    bool get_pin(uint8_t pin);

    /**
     * @brief Sets a mask to selectively enable pins in the shift register.
     * @param mask The mask value.
     */
    void set_mask(uint8_t mask);

    /**
     * @brief Clears a mask to selectively disable pins in the shift register.
     * @param mask The mask value.
     */
    void clear_mask(uint8_t mask);
    
};

#endif // SHIFT_REGISTER_H
