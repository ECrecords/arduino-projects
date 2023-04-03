#include <Arduino.h>

#define BTN0 2
#define BTN1 4

#define LEDR 3
#define LEDB 5

volatile bool intr0_triggered = false;
void isr0()
{
     intr0_triggered = true;
}

volatile bool intr1_triggered = false;
void isr1()
{
     intr1_triggered = true;
}

void debounce_call_func(uint8_t pin, uint32_t debounce_time, volatile bool &interrupt_flag, voidFuncPtr func, uint32_t &last_debounce_time, PinStatus desired_state = LOW)
{
     uint32_t current_time;

     if (interrupt_flag)
     {
          current_time = millis();

          if ((current_time - last_debounce_time) >= debounce_time)
          {
               if (digitalRead(pin) == desired_state)
               {
                    func();
               }

               interrupt_flag = false;
               last_debounce_time = current_time;
          }
     }
}

inline void ledr()
{
     digitalWrite(LEDR, !(digitalRead(LEDR)));
}

inline void ledb()
{
     digitalWrite(LEDB, !(digitalRead(LEDB)));
}

inline void ledrb()
{
     ledr();
     ledb();
}

void setup()
{
     Serial.begin(PICO_DEFAULT_UART_BAUD_RATE);

     attachInterrupt(digitalPinToInterrupt(BTN0), isr0, CHANGE);
     pinMode(BTN0, INPUT_PULLUP);

     attachInterrupt(digitalPinToInterrupt(BTN1), isr1, CHANGE);
     pinMode(BTN1, INPUT_PULLUP);

     delayMicroseconds(50);

     pinMode(LEDR, OUTPUT);
     pinMode(LEDB, OUTPUT);
}

uint32_t ldt0, ldt1 = 0;
void loop()
{
#ifdef PROGRAM1
     debounce_call_func(BTN0, 50, intr0_triggered, ledr, ldt0, LOW);
#endif

#ifdef PROGRAM2
     debounce_call_func(BTN0, 50, intr0_triggered, ledrb, ldt0, LOW);
#endif

#ifdef PROGRAM3
     debounce_call_func(BTN0, 50, intr0_triggered, ledr, ldt0, LOW);
     debounce_call_func(BTN1, 50, intr1_triggered, ledb, ldt1, LOW);
#endif

#ifdef PROGRAM4
     debounce_call_func(BTN0, 50, intr0_triggered, ledrb, ldt0, HIGH);
     debounce_call_func(BTN1, 50, intr1_triggered, ledrb, ldt1, HIGH);
#endif
}