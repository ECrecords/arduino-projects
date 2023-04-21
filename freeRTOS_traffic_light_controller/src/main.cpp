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
const TickType_t xYellowDuration = pdMS_TO_TICKS(2000);
const TickType_t xGreenDuration = pdMS_TO_TICKS(5000);
const TickType_t xRedGreenDuration = pdMS_TO_TICKS(2500);

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

// Toggle the North-South priority state when the North-South sensor is triggered
void toggleNorthSouthPriority(void *pvParameters)
{
    // Save the handle of the current task for use with notifications
    xNorthSouthTaskToNotify = xTaskGetCurrentTaskHandle();

    while (1)
    {
        // Wait for a notification from the North-South sensor interrupt service routine
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Toggle the North-South priority state
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState.northSouthPriority = !currentState.northSouthPriority;
        xSemaphoreGive(stateMutex);
    }
}

// Toggle the East-West priority state when the East-West sensor is triggered
void toggleEastWestPriority(void *pvParameters)
{
    // Save the handle of the current task for use with notifications
    xEastWestTaskToNotify = xTaskGetCurrentTaskHandle();

    while (1)
    {
        // Wait for a notification from the East-West sensor interrupt service routine
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Toggle the East-West priority state
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState.eastWestPriority = !currentState.eastWestPriority;
        xSemaphoreGive(stateMutex);
    }
}

// Traffic light handler function to set the on and off masks for the shift register and update the state of the lights
inline void handleTrafficLight(uint8_t onMask, uint8_t offMask)
{
    // Take the semaphore to access the shift register
    xSemaphoreTake(shiftRegisterAccess, portMAX_DELAY);

    // Clear the pins for the lights that need to be turned off
    lightResource.clear_mask(offMask);

    // Set the pins for the lights that need to be turned on
    lightResource.set_mask(onMask);

    // Release the semaphore to allow other tasks to access the shift register
    xSemaphoreGive(shiftRegisterAccess);
}

// Function to get the current intersection state from the shared state variable
inline void getIntersectionState(IntersectionState_t &readState)
{
    // Take the semaphore to access the shared state variable
    xSemaphoreTake(stateMutex, portMAX_DELAY);

    // Read the current intersection state and store it in the parameter
    readState = currentState;

    // Release the semaphore to allow other tasks to access the shared state variable
    xSemaphoreGive(stateMutex);
}

// Function to update the state of the traffic light in the shared state variable
inline void updateIntersectionState(TrafficLightState_t &target, TrafficLightState_t update)
{
    // Take the semaphore to access the shared state variable
    xSemaphoreTake(stateMutex, portMAX_DELAY);

    // Update the state of the traffic light in the shared state variable
    target = update;

    // Release the semaphore to allow other tasks to access the shared state variable
    xSemaphoreGive(stateMutex);
}
void northSouthLight(void *pvParameters)
{
    // Initialize delay duration, last updated time, and intersection state
    TickType_t xDelayDuration = pdMS_TO_TICKS(0);
    TickType_t xLastUpdatedTime = xTaskGetTickCount();
    IntersectionState_t readState;

    // Initialize onLights, offLights, and current tick count
    uint8_t onLights;
    uint8_t offLights;
    TickType_t xCurrentTickCount;

    for (;;)
    {
        // Get intersection state and current tick count
        getIntersectionState(readState);
        xCurrentTickCount = xTaskGetTickCount();

        // Update north-south lights based on intersection state
        if (((xCurrentTickCount - xLastUpdatedTime) >= xDelayDuration) && !readState.northSouthPriority && !readState.eastWestPriority)
        {
            // Update lights based on current north-south state
            switch (readState.northSouth)
            {
            case TrafficLightState_t::GREEN:
                // Set delay duration, onLights, offLights, and next state
                xDelayDuration = xGreenDuration;
                onLights = 1 << GREEN_NS;
                offLights = 1 << RED_NS | 1 << YELLOW_NS;
                readState.northSouth = TrafficLightState_t::YELLOW;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::YELLOW:
                // Set delay duration, onLights, offLights, and next state
                xDelayDuration = xYellowDuration;
                onLights = 1 << YELLOW_NS;
                offLights = 1 << RED_NS | 1 << GREEN_NS;
                readState.northSouth = TrafficLightState_t::RED;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::RED:
                // If east-west is also red, update lights and set delay duration
                if (readState.eastWest == TrafficLightState_t::RED)
                {
                    readState.northSouth = TrafficLightState_t::GREEN;
                    xLastUpdatedTime = xCurrentTickCount;
                    xDelayDuration = xRedGreenDuration;
                }
                // Otherwise, just update lights
                onLights = 1 << RED_NS;
                offLights = 1 << GREEN_NS | 1 << YELLOW_NS;
                break;
            }

            // Update intersection state and lights
            updateIntersectionState(currentState.northSouth, readState.northSouth);
            handleTrafficLight(onLights, offLights);
        }
        // If north-south priority is active, set green lights
        else if (readState.northSouthPriority)
        {
            readState.northSouth = TrafficLightState_t::GREEN;
            onLights = 1 << GREEN_NS;
            offLights = 1 << RED_NS | 1 << YELLOW_NS;
            // Update intersection state and lights
            updateIntersectionState(currentState.northSouth, readState.northSouth);
            handleTrafficLight(onLights, offLights);
        }
        // If east-west priority is active, set red lights
        else if (readState.eastWestPriority)
        {
            readState.northSouth = TrafficLightState_t::RED;
            onLights = 1 << RED_NS;
            offLights = 1 << GREEN_NS | 1 << YELLOW_NS;
            // Update intersection state and lights
            updateIntersectionState(currentState.northSouth, readState.northSouth);
            handleTrafficLight(onLights, offLights);
        }
    }
}

void eastWestLight(void *pvParameters)
{
    // Initialize delay duration, last updated time, and intersection state
    TickType_t xDelayDuration = pdMS_TO_TICKS(0);
    TickType_t xLastUpdatedTime = xTaskGetTickCount();
    IntersectionState_t readState;

    // Initialize onLights, offLights, and current tick count
    uint8_t onLights;
    uint8_t offLights;
    TickType_t xCurrentTickCount;

    for (;;)
    {
        // Get intersection state and current tick count
        getIntersectionState(readState);
        xCurrentTickCount = xTaskGetTickCount();

        // If no priority is active and the time since the last update has exceeded the delay duration, update the east-west light states
        if (((xCurrentTickCount - xLastUpdatedTime) >= xDelayDuration) && !readState.northSouthPriority && !readState.eastWestPriority)
        {
            // Update lights based on current east-west state
            switch (readState.eastWest)
            {
            case TrafficLightState_t::GREEN:
                // Set delay duration, onLights, offLights, and next state
                xDelayDuration = xGreenDuration;
                onLights = 1 << GREEN_EW;
                offLights = 1 << RED_EW | 1 << YELLOW_EW;
                readState.eastWest = TrafficLightState_t::YELLOW;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::YELLOW:
                // Set delay duration, onLights, offLights, and next state
                xDelayDuration = xYellowDuration;
                onLights = 1 << YELLOW_EW;
                offLights = 1 << RED_EW | 1 << GREEN_EW;
                readState.eastWest = TrafficLightState_t::RED;
                xLastUpdatedTime = xCurrentTickCount;
                break;
            case TrafficLightState_t::RED:
                // If north-south is also red, update lights and set delay duration
                if (readState.northSouth == TrafficLightState_t::RED)
                {
                    readState.eastWest = TrafficLightState_t::GREEN;
                    xLastUpdatedTime = xCurrentTickCount;
                    xDelayDuration = xRedGreenDuration;
                }
                // Otherwise, just update lights
                onLights = 1 << RED_EW;
                offLights = 1 << GREEN_EW | 1 << YELLOW_EW;
                break;
            }

            // Update intersection state and lights
            updateIntersectionState(currentState.eastWest, readState.eastWest);
            handleTrafficLight(onLights, offLights);
        }
        // If north-south priority is active, set east-west red lights
        else if (readState.northSouthPriority)
        {
            readState.eastWest = TrafficLightState_t::RED;
            onLights = 1 << RED_EW;
            offLights = 1 << GREEN_EW | 1 << YELLOW_EW;

            // Update intersection state and lights
            updateIntersectionState(currentState.eastWest, readState.eastWest);
            handleTrafficLight(onLights, offLights);
        }
        // If east-west priority is active, set east-west green lights
        else if (readState.eastWestPriority)
        {
            readState.eastWest = TrafficLightState_t::GREEN;
            onLights = 1 << GREEN_EW;
            offLights = 1 << RED_EW | 1 << YELLOW_EW;

            // Update intersection state and lights
            updateIntersectionState(currentState.eastWest, readState.eastWest);
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
