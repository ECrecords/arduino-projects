# Debouncing Button Inputs with Interrupts

This Arduino sketch demonstrates how to debounce button inputs using interrupts and a debounce function. It also shows how to toggle LED outputs in response to button inputs. There are four different programs included, which can be selected by defining different preprocessor macros at compile time. The programs demonstrate different ways to use the debounce function and toggle the LEDs in response to button inputs.
## Getting Started

To use this code, you will need an Arduino board and the Arduino IDE. Follow these steps to get started:

1. Connect a button to `BTN0` or `BTN1` and connect an LED to `LEDR` or `LEDB`.
2.  Open the Arduino IDE and create a new sketch.
3.  Copy the code from debounce_button_inputs.ino into the new sketch.
4.  Select the appropriate program by defining the corresponding preprocessor macro (`PROGRAM1`, `PROGRAM2`, `PROGRAM3`, or `PROGRAM4`) at the top of the sketch.
5.  Compile and upload the sketch to your Arduino board.

## How to Use

Press the button and the corresponding LED should toggle on and off. The debounce time can be adjusted by changing the value passed to debounce_time in the debounce_call_func function.
## Programs

There are four different programs included in this sketch:
### Program 1

This program debounces `BTN0` and toggles `LEDR` when the button is pressed.

```c++

#define PROGRAM1
debounce_call_func(BTN0, 50, intr0_triggered, ledr, ldt0, HIGH);
```

### Program 2

This program debounces `BTN0` and toggles `LEDR` and `LEDB` when the button is pressed.

```c++

#define PROGRAM2
debounce_call_func(BTN0, 50, intr0_triggered, ledrb, ldt0, HIGH);
```
### Program 3

This program debounces both `BTN0` and `BTN1` and toggles `LEDR` when `BTN0` is pressed and `LEDB` when `BTN1` is pressed.

```c++

#define PROGRAM3
debounce_call_func(BTN0, 50, intr0_triggered, ledr, ldt0, HIGH);
debounce_call_func(BTN1, 50, intr1_triggered, ledb, ldt1, HIGH);
```

### Program 4

This program debounces both `BTN0` and `BTN1` and toggles `LEDR` and `LEDB` when either button is pressed.

```c++

#define PROGRAM4
debounce_call_func(BTN0, 50, intr0_triggered, ledrb, ldt0, HIGH);
debounce_call_func(BTN1, 50, intr1_triggered, ledrb, ldt1, HIGH);
```
## Author