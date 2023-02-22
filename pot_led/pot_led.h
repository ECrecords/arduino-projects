#include <pins_arduino.h>

#define EXTERNAL_LED_PIN    PIN3
#define POTENTIOMETER_PIN  A0

#define BAUD_RATE 115200

#define MAX_DUTY 255
#define MIN_DUTY 0

#define UINT10_MAX 1023
#define BUFFER_SZ 126

#define CALIBRATION_MODE_PERIOD_MS 5000
#define CALIBRATION_MESSAGE "----- ENTERING CALIBRATION MODE -----\nMove potentialmeter to maximum and minimum potions multiple times in 5 seconds."