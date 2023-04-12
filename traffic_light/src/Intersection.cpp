#include <Intersection.h>

Intersection::Intersection()
{
}

Intersection::Intersection(uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin) : data_pin(data_pin), latch_pin(latch_pin), clock_pin(clock_pin), init(true)
{
     pinMode(data_pin, OUTPUT);
     pinMode(latch_pin, OUTPUT);
     pinMode(clock_pin, OUTPUT);
}



void Intersection::state_output()
{
     switch (current_state)
     {
     case GO1_STOP2:
          update_time = 1000;
          break;

     case SLOW1_STOP2:
          update_time = 1000;
          break;

     case STOP1_GO2:
          update_time = 1000;
          break;

     case STOP1_SLOW2:
          update_time = 1000;
          break;

     case STOP1_STOP2:
          update_time = 1000;
          break;

     case ERROR_BLINK_ON:
          update_time = 1000;
          current_state = ERROR_BLINK_OFF;
          break;

     case ERROR_BLINK_OFF:
          update_time = 1000;
          current_state = ERROR_BLINK_ON;
          break;

     default:
          update_time = 1000;
          current_state = ERROR_BLINK_ON;
          break;
     }


     digitalWrite(latch_pin, LOW);
     shiftOut(data_pin, clock_pin, MSBFIRST, shift_data[current_state]);
     digitalWrite(latch_pin, HIGH);    
}

void Intersection::update_state()
{
     switch (previous_state)
     {
     case GO1_STOP2:
          update_time = 1000;
          current_state = SLOW1_STOP2;
          break;
     case SLOW1_STOP2:
          update_time = 1000;
          current_state = STOP1_STOP2;
          break;
     case STOP1_GO2:
          update_time = 1000;
          current_state = STOP1_SLOW2;
          break;
     case STOP1_SLOW2:
          update_time = 1000;
          current_state = STOP1_STOP2;
          break;
     case STOP1_STOP2:
          update_time = 1000;
          if (previous_state == STOP1_SLOW2)
          {
               current_state = GO1_STOP2;
               break;
          }

          if (previous_state == SLOW1_STOP2)
          {
               current_state = STOP1_GO2;
               break;
          }

          current_state = ERROR_BLINK_ON;
          break;

     case ERROR_BLINK_ON:
          update_time = 1000;
          current_state = ERROR_BLINK_OFF;
          break;

     case ERROR_BLINK_OFF:
          update_time = 1000;
          current_state = ERROR_BLINK_ON;
          break;

     default:
          update_time = 1000;
          current_state = ERROR_BLINK_ON;
          break;
     }

     previous_state = current_state;
}

void Intersection::run()
{
     uint32_t current_time = millis();

     if (!init)
          return;

     if ((current_time - measured_time) == update_time)
     {
          update_state();
          measured_time = current_time;
     }

     state_output();
}

void Intersection::set_state_bytes(uint8_t bytes[7]) 
{
     for (uint8_t i = 0; i < 7; i++)
     {
          shift_data[i] = bytes[i];
     }
}
