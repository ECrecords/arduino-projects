#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <shift_register.h>
#include <semphr.h>
#include <task.h>

// Total number of states in the traffic light cycle
#define TOTAL_STATES 6

// Define the bit patterns for the North-South and East-West traffic light LEDs
// connected to the shift register pins
#define RED_NS 1
#define YELLOW_NS 2
#define GREEN_NS 3

#define RED_EW 4
#define YELLOW_EW 5
#define GREEN_EW 6

// Define pins for the shift register
#define DATA_PIN 19
#define LATCH_PIN 18
#define CLOCK_PIN 17

#define NS_SENSE 2
#define EW_SENSE 4

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

enum class TrafficLightState_t
{
    GREEN,
    YELLOW,
    RED
};

struct IntersectionState_t
{
    TrafficLightState_t northSouth;
    TrafficLightState_t eastWest;
};

ShiftRegister lightResource;
SemaphoreHandle_t shiftRegisterAccess;
SemaphoreHandle_t stateMutex;
IntersectionState_t currentState;

const TickType_t xRedDuration = pdMS_TO_TICKS(2000);
const TickType_t xYellowDuration = pdMS_TO_TICKS(2000);
const TickType_t xGreenDuration = pdMS_TO_TICKS(5000);
const TickType_t xErrorDuration = pdMS_TO_TICKS(1000);

void northSouthLight(void *pvParameters)
{
    TickType_t xDelayDuration;
    uint8_t setMask;
    uint8_t clearMask;
    IntersectionState_t readState;

    for (;;)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        Serial.println("accquired state in northSourth Task");
        readState = currentState;
        xSemaphoreGive(stateMutex);

        switch (readState.northSouth)
        {
        case TrafficLightState_t::GREEN:
            readState.northSouth = TrafficLightState_t::YELLOW;
            setMask = 1 << GREEN_NS | 0 << YELLOW_NS | 0 << RED_NS;
            clearMask = 0 << GREEN_NS | 1 << YELLOW_NS | 1 << RED_NS;
            xDelayDuration = xGreenDuration;
            break;
        case TrafficLightState_t::YELLOW:
            readState.northSouth = TrafficLightState_t::RED;
            setMask = 0 << GREEN_NS | 1 << YELLOW_NS | 0 << RED_NS;
            clearMask = 1 << GREEN_NS | 0 << YELLOW_NS | 1 << RED_NS;
            xDelayDuration = xYellowDuration;
            break;

        case TrafficLightState_t::RED:
            readState.northSouth = TrafficLightState_t::GREEN;
            setMask = 0 << GREEN_NS | 0 << YELLOW_NS | 1 << RED_NS;
            clearMask = 1 << GREEN_NS | 1 << YELLOW_NS | 0 << RED_NS;
            xDelayDuration = xRedDuration;
            break;
        default:
            readState.northSouth = TrafficLightState_t::RED;
            xDelayDuration = xErrorDuration;
            break;
        }

        xSemaphoreTake(shiftRegisterAccess, portMAX_DELAY);
        lightResource.clear_mask(clearMask);
        lightResource.set_mask(setMask);
        xSemaphoreGive(shiftRegisterAccess);

        vTaskDelay(xDelayDuration);

        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState = readState;
        xSemaphoreGive(stateMutex);
    }
}

void eastWestLight(void *pvParameters)
{
    IntersectionState_t readState;
    TickType_t xDelayDuration;

    uint8_t setMask, clearMask;

    for (;;)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        Serial.println("accquired state in eastWest Task");
        readState = currentState;
        xSemaphoreGive(stateMutex);

        if (readState.northSouth == TrafficLightState_t::YELLOW)
        {
            readState.eastWest = TrafficLightState_t::GREEN;
            setMask = 1 << GREEN_EW | 0 << YELLOW_EW | 0 << RED_EW;
            clearMask = 0 << GREEN_EW | 1 << YELLOW_EW | 1 << RED_EW;
            xDelayDuration = xGreenDuration;
        }
        else
        {
            readState.eastWest = TrafficLightState_t::RED;
            setMask = 0 << GREEN_EW | 0 << YELLOW_EW | 1 << RED_EW;
            clearMask = 1 << GREEN_EW | 1 << YELLOW_EW | 0 << RED_EW;
            xDelayDuration = xRedDuration;
        }

        xSemaphoreTake(shiftRegisterAccess, portMAX_DELAY);
        lightResource.clear_mask(clearMask);
        lightResource.set_mask(setMask);
        xSemaphoreGive(shiftRegisterAccess);

        vTaskDelay(xDelayDuration);

        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState = readState;
        xSemaphoreGive(stateMutex);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(NS_SENSE, INPUT_PULLUP);
    pinMode(EW_SENSE, INPUT_PULLUP);

    lightResource = ShiftRegister(CLOCK_PIN, LATCH_PIN, DATA_PIN);
    shiftRegisterAccess = xSemaphoreCreateMutex();
    stateMutex = xSemaphoreCreateMutex();
    currentState = {TrafficLightState_t::GREEN, TrafficLightState_t::RED};

    attachInterrupt(digitalPinToInterrupt(NS_SENSE), ns_sense_isr, FALLING);

    xTaskCreate(northSouthLight, "North South Light", 128, NULL, 1, NULL);
    xTaskCreate(eastWestLight, "East West Light", 128, NULL, 1, NULL);
    xTaskCreate(sense_handler_task, "Sense Handler", 128, NULL, 2, NULL);

    vTaskStartScheduler();

    xSemaphoreGive(shiftRegisterAccess);
    xSemaphoreGive(stateMutex);
}

void loop() {}
