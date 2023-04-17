#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <shift_register.h>

// Total number of states in the traffic light cycle
#define TOTAL_STATES 6

// Define the bit patterns for the North-South and East-West traffic light LEDs
// connected to the shift register pins
#define RED_NS 0x02
#define YELLOW_NS 0x04
#define GREEN_NS 0x08

#define RED_EW 0x10
#define YELLOW_EW 0x20
#define GREEN_EW 0x40

// Define pins for the shift register
#define DATA_PIN 19
#define LATCH_PIN 18
#define CLOCK_PIN 17

#define NS_SENSE 2
#define EW_SENSE 4

inline void write_to_shiftregister(uint8_t val)
{
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, val);
    digitalWrite(LATCH_PIN, HIGH);
}

// Define the output states and durations for each state in the traffic light cycle
const uint8_t output_data[TOTAL_STATES]{
    RED_NS | GREEN_EW,
    RED_NS | YELLOW_EW,
    RED_NS | RED_EW,
    GREEN_NS | RED_EW,
    YELLOW_NS | RED_EW,
    RED_NS | RED_EW};

uint32_t switch_time[TOTAL_STATES]{
    5000,
    2500,
    500,
    5000,
    2500,
    500};

void traffic_controller(void *pvParameters)
{
    for (;;)
    {
        // Iterate through each state in the traffic light cycle
        for (uint8_t i = 0; i < TOTAL_STATES; i++)
        {
            // Send the current state to the shift register
            write_to_shiftregister(output_data[i]);

            // Keep the current state active until its switch_time duration has passed
            vTaskDelay(pdMS_TO_TICKS(switch_time[i]));
        }
    }
}

void blinky(void *pvParameters)
{
    for (;;)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

TaskHandle_t xTaskToNotify = NULL;

void ns_sense_isr()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void sense_handler_task(void *pvParameters)
{
    xTaskToNotify = xTaskGetCurrentTaskHandle();

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for the notification
        
        Serial.println("Car at North-South Traffic Light");
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(NS_SENSE, INPUT_PULLUP);
    pinMode(EW_SENSE, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(NS_SENSE), ns_sense_isr, FALLING);

    xTaskCreate(traffic_controller, "Traffic Light Controller", 128, NULL, 1, NULL);
    xTaskCreate(blinky, "Blinky", 128, NULL, 1, NULL);
    xTaskCreate(sense_handler_task, "Sense Handler", 128, NULL, 2, NULL);

    vTaskStartScheduler();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
