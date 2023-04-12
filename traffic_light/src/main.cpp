#include <Arduino.h>

#define RED_NS 0x02
#define YELLOW_NS 0x04
#define GREEN_NS 0x08

#define RED_WE 0x10
#define YELLOW_WE 0x20
#define GREEN_WE 0x40

#define DATA_PIN 19
#define LATCH_PIN 18
#define CLOCK_PIN 17

uint8_t output_data[6]{
    RED_NS | GREEN_WE,
    RED_NS | YELLOW_WE,
    RED_NS | RED_WE,
    GREEN_NS | RED_WE,
    YELLOW_NS | RED_WE,
    RED_NS | RED_WE};

uint32_t switch_time[6] {
     5000,
     2500,
     500,
     5000,
     2500,
     500
};

void setup()
{
     pinMode(DATA_PIN, OUTPUT);
     pinMode(LATCH_PIN, OUTPUT);
     pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
     uint32_t current_time;;
     uint32_t last_upated_time;
     bool set;

     for (uint8_t i = 0; i < 6; i++)
     {
          current_time = millis();
          last_upated_time = millis();
          set = false;
          while((current_time - last_upated_time) != switch_time[i]){
               current_time = millis();

               if (!set)
               {
                    set = true;
                    digitalWrite(LATCH_PIN, LOW);
                    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, output_data[i]);
                    digitalWrite(LATCH_PIN, HIGH);     
               }
               
          }
     }
}