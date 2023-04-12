#pragma once
#include <Arduino.h>

class Intersection
{
     enum state_t
     {
          GO1_STOP2 = 0,
          SLOW1_STOP2,
          STOP1_STOP2,
          STOP1_GO2,
          STOP1_SLOW2,
          ERROR_BLINK_ON,
          ERROR_BLINK_OFF
     };

private:
     uint8_t shift_data[7];
     state_t current_state = STOP1_STOP2;
     state_t previous_state = SLOW1_STOP2;
     uint8_t data_pin;
     uint8_t latch_pin;
     uint8_t clock_pin;
     bool init;
     uint32_t measured_time;
     uint32_t update_time;

public:
     /// @brief
     /// @param data_pin
     /// @param latch_pin
     /// @param clock_pin
     Intersection(uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin);

     Intersection();

     void run();
     void set_state_bytes(uint8_t bytes[7]);

private:
     void state_output();

     void update_state();
};