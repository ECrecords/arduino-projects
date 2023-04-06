#include <Arduino.h>

// Define constants for pin numbers
#define BTN0 2
#define BTN1 4
#define LEDR 3
#define LEDB 5

// Declare two interrupt service routines for the two buttons
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

// Define a function to debounce a button press
void debounce_call_func(uint8_t pin, uint32_t debounce_time, volatile bool &interrupt_flag, voidFuncPtr func, uint32_t &last_debounce_time, PinStatus desired_state = LOW)
{
     uint32_t current_time;

     // If the button has triggered an interrupt
     if (interrupt_flag)
     {
          // Record the current time
          current_time = millis();

          // If the debounce time has passed since the last check
          if ((current_time - last_debounce_time) >= debounce_time)
          {
               // If the button state matches the desired state
               if (digitalRead(pin) == desired_state)
               {
                    // Call the desired function
                    func();
               }

               // Reset the interrupt flag and the last debounce time
               interrupt_flag = false;
               last_debounce_time = current_time;
          }
     }
}

// Define inline functions to toggle the state of the LED pins
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
     // Initialize the serial communication with the default baud rate
     Serial.begin(PICO_DEFAULT_UART_BAUD_RATE);

     // Attach interrupts to the two buttons
     attachInterrupt(digitalPinToInterrupt(BTN0), isr0, CHANGE);
     pinMode(BTN0, INPUT_PULLUP);

     attachInterrupt(digitalPinToInterrupt(BTN1), isr1, CHANGE);
     pinMode(BTN1, INPUT_PULLUP);

     // Wait for the inputs to stabilize
     delayMicroseconds(50);

     // Set the LED pins as outputs
     pinMode(LEDR, OUTPUT);
     pinMode(LEDB, OUTPUT);
}

// Declare variables to keep track of the debounce time for each button
uint32_t ldt0, ldt1 = 0;

void loop()
{
     // Choose which program to run based on the selected preprocessor directive
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
