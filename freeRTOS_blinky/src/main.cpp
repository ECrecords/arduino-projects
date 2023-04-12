// /**
//  * @ Author: Elvis Chino-Islas
//  * @ Modified by: Elvis Chino-Islas
//  * @ Description: A program that blinks two LEDs (red and blue) with different priorities using RTOS.
//  */

#include <Arduino.h>

void setup(){}

void loop(){}

// using namespace std::chrono;

// // Define threads and their priorities
// rtos::Thread task1;
// osPriority_t task1_priority = osPriorityLow;

// rtos::Thread task2;
// osPriority_t task2_priority = osPriorityHigh;

// // Define LED pins
// #define RED_LED 3
// #define BLUE_LED 5

// // Task 1 function: control the red LED
// void task1_func(void)
// {
//     // Define period for blinking
//     milliseconds period_ms = 500ms;

//     // Infinite loop for blinking the red LED
//     for (;;)
//     {
//         digitalWrite(RED_LED, HIGH); // Turn the red LED on
//         rtos::ThisThread::sleep_for(period_ms / 2); // Wait half the period
//         digitalWrite(RED_LED, LOW); // Turn the red LED off
//         rtos::ThisThread::sleep_for(period_ms / 2); // Wait the other half of the period
//     }
// }

// // Task 2 function: control the blue LED
// void task2_func()
// {
//     // Define period for blinking
//     milliseconds period_ms = 500ms;

//     // Infinite loop for blinking the blue LED
//     for (;;)
//     {
//         digitalWrite(BLUE_LED, HIGH); // Turn the blue LED on
//         rtos::ThisThread::sleep_for(period_ms / 2); // Wait half the period
//         digitalWrite(BLUE_LED, LOW); // Turn the blue LED off
//         rtos::ThisThread::sleep_for(period_ms / 2); // Wait the other half of the period
//     }
// }

// // Setup function: configure pins and start tasks
// void setup()
// {
//     // Configure LED pins as outputs
//     pinMode(RED_LED, OUTPUT);
//     pinMode(BLUE_LED, OUTPUT);

//     // Start Task 1 and set its priority
//     task1.start(mbed::callback(task1_func));
//     task1.set_priority(task1_priority);

//     // Start Task 2 and set its priority
//     task2.start(mbed::callback(task2_func));
//     task2.set_priority(task2_priority);
// }

// // Main loop function: keep the main thread alive
// void loop()
// {
//     // Sleep the main thread for 2 milliseconds
//     rtos::ThisThread::sleep_for(2ms);
// }
