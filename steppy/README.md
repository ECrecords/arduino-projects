# Warm Up Project 1

## Submission Details
**Author:** Elvis Chino-Islas  
**Course:** COMP522  
**Course Name:** Embedded System Applications  
**Completion Date:** 30 March 2023
# Stepper Motor Speed Control using State Machine and Arduino

## Introduction

This project demonstrates an Arduino-based embedded application designed to control the speed and direction of a stepper motor using a state-machine approach and two push-buttons as inputs. The primary objective of this project is to develop an understanding of state-machine architecture and apply it to a practical scenario involving stepper motor control.

## Implementation

The key components of the stepper motor control system implementation are outlined below:

### 1. State-machine architecture

The implementation utilizes a state-machine approach with an enumeration of state codes representing the different states of the system. A state transition matrix is used to define the possible transitions for each state, making it easy to manage state transitions in the code. The current state of the program is stored in the `curr_state` variable, which is initialized to `STOP_INIT`.

### 2. Input handling

Two input pins, INPUT1 and INPUT2, are set up as pull-up inputs. Interrupts are utilized to handle the push-buttons, improving the system's responsiveness. The interrupt handlers `handle_input1()` and `handle_input2()` set the respective flags `input1_intr` and `input2_intr` when an interrupt event is triggered. The `det_state()` function is used to determine the current input event by checking these flags and resetting them after detection.

### 3. Motor control

The A4988 driver is used to control the stepper motor in this implementation. An instance of the A4988 class, `hw`, is created to manage motor control. The motor is initialized with a default speed in the `setup()` function using `hw.init()`. The `hw.sstep()` function is called in the `loop()` function to perform a single step of the motor.

### 4. State functions

Two state functions, `stopped()` and `update_speed()`, are implemented to handle the different states of the system. The `stopped()` function stops the motor and detects input events. The `update_speed()` function updates the motor's speed based on the current state and starts the motor if it was stopped. Both functions return the detected input event as a `ret_code` enumeration value.

### 5. Main loop

The `loop()` function serves as the main control loop for the state machine. It calls the appropriate state function based on the current state and updates the current state using the state transition matrix. The current state and motor speed are printed to the serial monitor for debugging and observation purposes.

## Usage

Upload the Arduino sketch to the microcontroller and connect the stepper motor to the A4988 driver. Wire up the push-buttons to the specified input pins and start controlling the motor's speed and direction using the INPUT1 and INPUT2 buttons.

## Conclusion

By combining these components, the implementation provides a flexible and efficient way to control the stepper motor's speed and direction using the state-machine architecture. The code demonstrates the ability to handle state transitions, input events, and motor control effectively, meeting the project requirements and providing a solid foundation for further improvements or expansions.
