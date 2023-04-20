/**
 * Traffic Light Control System using FreeRTOS
 * Author: ELvis Chino-Islas
 *
 * This program is designed to control the traffic lights at an intersection
 * using FreeRTOS tasks and semaphores. The traffic lights have priority
 * sensors that trigger the lights to change based on the direction of traffic.
 */

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
#define EW_SENSE 3

// Enum class representing the different states of the traffic light
enum class TrafficLightState_t
{
    GREEN,
    YELLOW,
    RED
};

// Struct representing the current state of the intersection
struct IntersectionState_t
{
    TrafficLightState_t northSouth;
    TrafficLightState_t eastWest;
    bool northSouthPriority;
    bool eastWestPriority;
};

ShiftRegister lightResource;
SemaphoreHandle_t shiftRegisterAccess;
SemaphoreHandle_t stateMutex;
IntersectionState_t currentState;

// Durations for each traffic light state
const TickType_t xRedDuration = pdMS_TO_TICKS(5000);
const TickType_t xYellowDuration = pdMS_TO_TICKS(2000);
const TickType_t xGreenDuration = pdMS_TO_TICKS(5000);
const TickType_t xRedGreenDuration = pdMS_TO_TICKS(3000);
const TickType_t xErrorDuration = pdMS_TO_TICKS(1000);

TaskHandle_t xNorthSouthTaskToNotify = NULL;
TaskHandle_t xEastWestTaskToNotify = NULL;
TickType_t lastNorthSouthInterruptTickCount = 0;
TickType_t lastEastWestInterruptTickCount = 0;

// North-South sensor interrupt service routine (ISR) with debouncing
void NorthSouthSensorISR()
{
    // Debounce time interval in ticks
    const TickType_t debounceTicks = pdMS_TO_TICKS(50);

    // Get the current tick count
    TickType_t currentTickCount = xTaskGetTickCountFromISR();

    // Check if the time elapsed since the last interrupt is greater than the debounce time
    if (currentTickCount - lastNorthSouthInterruptTickCount >= debounceTicks)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(xNorthSouthTaskToNotify, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }

    // Update the last interrupt tick count
    lastNorthSouthInterruptTickCount = currentTickCount;
}

// East-West sensor interrupt service routine (ISR) with debouncing
void EastWestSensorISR()
{
    // Debounce time interval in ticks
    const TickType_t debounceTicks = pdMS_TO_TICKS(50);

    // Get the current tick count
    TickType_t currentTickCount = xTaskGetTickCountFromISR();

    // Check if the time elapsed since the last interrupt is greater than the debounce time
    if (currentTickCount - lastEastWestInterruptTickCount >= debounceTicks)
    {
        Serial.println("East-West Sensor tiggered");
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(xEastWestTaskToNotify, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }

    // Update the last interrupt tick count
    lastEastWestInterruptTickCount = currentTickCount;
}

void toggleNorthSouthPriority(void *pvParameters)
{
    xNorthSouthTaskToNotify = xTaskGetCurrentTaskHandle();

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for the notification

        Serial.println("Toggled North-South Priority");
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState.northSouthPriority = !currentState.northSouthPriority;
        xSemaphoreGive(stateMutex);
    }
}

void toggleEastWestPriority(void *pvParameters)
{
    xEastWestTaskToNotify = xTaskGetCurrentTaskHandle();

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for the notification
        Serial.println("Toggled East-West Priority");
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState.eastWestPriority = !currentState.eastWestPriority;
        xSemaphoreGive(stateMutex);
    }
}

inline void handleTrafficLight(uint8_t onMask, uint8_t offMask)
{
    xSemaphoreTake(shiftRegisterAccess, portMAX_DELAY);
    lightResource.clear_mask(offMask);
    lightResource.set_mask(onMask);
    xSemaphoreGive(shiftRegisterAccess);
}

inline void getIntersectionState(IntersectionState_t &readState)
{
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    readState = currentState;
    xSemaphoreGive(stateMutex);
}

inline void updateIntersectionState(IntersectionState_t state)
{
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    currentState = state;
    xSemaphoreGive(stateMutex);
}

void northSouthLight(void *pvParameters)
{
    TickType_t xDelayDuration = pdMS_TO_TICKS(0);
    TickType_t xLastUpdatedTime = xTaskGetTickCount();
    IntersectionState_t readState;

    uint8_t onLights;
    uint8_t offLights;

    TickType_t xCurrentTickCount;

    for (;;)
    {
        getIntersectionState(readState);

        xCurrentTickCount = xTaskGetTickCount();

        if (((xCurrentTickCount - xLastUpdatedTime) >= xDelayDuration) && !readState.northSouthPriority && !readState.eastWestPriority)
        {

            switch (readState.northSouth)
            {
            case TrafficLightState_t::GREEN:
                xDelayDuration = xGreenDuration;
                onLights = 1 << GREEN_NS;
                offLights = 1 << RED_NS | 1 << YELLOW_NS;
                readState.northSouth = TrafficLightState_t::YELLOW;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::YELLOW:
                xDelayDuration = xYellowDuration;
                onLights = 1 << YELLOW_NS;
                offLights = 1 << RED_NS | 1 << GREEN_NS;
                readState.northSouth = TrafficLightState_t::RED;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::RED:
                if (readState.eastWest == TrafficLightState_t::RED)
                {
                    readState.northSouth = TrafficLightState_t::GREEN;
                    xLastUpdatedTime = xCurrentTickCount;
                    xDelayDuration = xRedGreenDuration;
                }
                onLights = 1 << RED_NS;
                offLights = 1 << GREEN_NS | 1 << YELLOW_NS;

                break;
            }

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.northSouth = readState.northSouth;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }

        else if (readState.northSouthPriority)
        {
            readState.northSouth = TrafficLightState_t::GREEN;
            onLights = 1 << GREEN_NS;
            offLights = 1 << RED_NS | 1 << YELLOW_NS;

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.northSouth = readState.northSouth;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }

        else if (readState.eastWestPriority)
        {
            readState.northSouth = TrafficLightState_t::RED;
            onLights = 1 << RED_NS;
            offLights = 1 << GREEN_NS | 1 << YELLOW_NS;

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.northSouth = readState.northSouth;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }
    }
}

void eastWestLight(void *pvParameters)
{
    TickType_t xDelayDuration = pdMS_TO_TICKS(0);
    TickType_t xLastUpdatedTime = xTaskGetTickCount();
    IntersectionState_t readState;

    uint8_t onLights;
    uint8_t offLights;

    TickType_t xCurrentTickCount;

    for (;;)
    {
        getIntersectionState(readState);

        xCurrentTickCount = xTaskGetTickCount();

        if (((xCurrentTickCount - xLastUpdatedTime) >= xDelayDuration) && !readState.northSouthPriority && !readState.eastWestPriority)
        {
            switch (readState.eastWest)
            {
            case TrafficLightState_t::GREEN:
                xDelayDuration = xGreenDuration;
                onLights = 1 << GREEN_EW;
                offLights = 1 << RED_EW | 1 << YELLOW_EW;
                readState.eastWest = TrafficLightState_t::YELLOW;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::YELLOW:
                xDelayDuration = xYellowDuration;
                onLights = 1 << YELLOW_EW;
                offLights = 1 << RED_EW | 1 << GREEN_EW;
                readState.eastWest = TrafficLightState_t::RED;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::RED:
                if (readState.northSouth == TrafficLightState_t::RED)
                {
                    readState.eastWest = TrafficLightState_t::GREEN;
                    xLastUpdatedTime = xCurrentTickCount;
                    xDelayDuration = xRedGreenDuration;
                }

                onLights = 1 << RED_EW;
                offLights = 1 << GREEN_EW | 1 << YELLOW_EW;

                break;
            }

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.eastWest = readState.eastWest;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }

        else if (readState.northSouthPriority)
        {
            readState.eastWest = TrafficLightState_t::RED;
            onLights = 1 << RED_EW;
            offLights = 1 << GREEN_EW | 1 << YELLOW_EW;

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.eastWest = readState.eastWest;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }
        else if (readState.eastWestPriority)
        {
            readState.eastWest = TrafficLightState_t::GREEN;
            onLights = 1 << GREEN_EW;
            offLights = 1 << RED_EW | 1 << YELLOW_EW;

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            currentState.eastWest = readState.eastWest;
            xSemaphoreGive(stateMutex);

            handleTrafficLight(onLights, offLights);
        }
    }
}

// Initialize the microcontroller pins and settings for the traffic light system
void setup()
{
    // Set up serial communication for debugging purposes
    Serial.begin(9600);

    // Configure the North-South and East-West sensor pins as input pull-ups
    pinMode(NS_SENSE, INPUT_PULLUP);
    pinMode(EW_SENSE, INPUT_PULLUP);

    // Initialize the shift register and the semaphores for controlling access to shared resources
    lightResource = ShiftRegister(CLOCK_PIN, LATCH_PIN, DATA_PIN);
    shiftRegisterAccess = xSemaphoreCreateMutex();
    stateMutex = xSemaphoreCreateMutex();

    // Set the initial states for the North-South and East-West traffic lights
    currentState.northSouth = TrafficLightState_t::GREEN;
    currentState.eastWest = TrafficLightState_t::RED;

    // Attach the interrupt service routines for the North-South and East-West sensors
    attachInterrupt(digitalPinToInterrupt(NS_SENSE), NorthSouthSensorISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(EW_SENSE), EastWestSensorISR, FALLING);

    // Create the tasks for managing the North-South and East-West traffic lights
    xTaskCreate(northSouthLight, "North South Light", 128, NULL, 1, NULL);
    xTaskCreate(eastWestLight, "East West Light", 128, NULL, 1, NULL);

    // Create the tasks for handling North-South and East-West sensor interrupts
    xTaskCreate(toggleNorthSouthPriority, "North South Sense Handler", 128, NULL, 2, NULL);
    xTaskCreate(toggleEastWestPriority, "East West Sense Handler", 128, NULL, 2, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Release the semaphores for the shift register and the shared state mutex
    xSemaphoreGive(shiftRegisterAccess);
    xSemaphoreGive(stateMutex);
}

// Main loop function - Not used in this application, as the tasks are managed by FreeRTOS
void loop() {}
