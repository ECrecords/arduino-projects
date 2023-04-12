#include <Arduino.h>

#define TOTAL_STATES 6

// Define the bit patterns for the North-South and West-East traffic light LEDs
// connected to the shift register pins
#define RED_NS 0x02
#define YELLOW_NS 0x04
#define GREEN_NS 0x08

#define RED_WE 0x10
#define YELLOW_WE 0x20
#define GREEN_WE 0x40

// Define pins for the shift register
#define DATA_PIN 19
#define LATCH_PIN 18
#define CLOCK_PIN 17

// Define the output states and durations for each state in the traffic light cycle
uint8_t output_data[TOTAL_STATES]{
    RED_NS | GREEN_WE,
    RED_NS | YELLOW_WE,
    RED_NS | RED_WE,
    GREEN_NS | RED_WE,
    YELLOW_NS | RED_WE,
    RED_NS | RED_WE};

uint32_t switch_time[TOTAL_STATES]{
    5000,
    2500,
    500,
    5000,
    2500,
    500};

void setup()
{
     // Set the pins for the shift register as output
     pinMode(DATA_PIN, OUTPUT);
     pinMode(LATCH_PIN, OUTPUT);
     pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
     uint32_t current_time;
     uint32_t last_upated_time;

     // Iterate through each state in the traffic light cycle
     for (uint8_t i = 0; i < TOTAL_STATES; i++)
     {
          current_time = millis();
          last_upated_time = millis();

          // Send the current state to the shift register
          digitalWrite(LATCH_PIN, LOW);
          shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, output_data[i]);
          digitalWrite(LATCH_PIN, HIGH);

          // Keep the current state active until its switch_time duration has passed
          while ((current_time - last_upated_time) < switch_time[i])
          {
               current_time = millis();
          }
     }
}
