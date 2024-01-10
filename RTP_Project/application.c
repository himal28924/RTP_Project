#include "application.h"
#include <stdio.h>  
#include <stdlib.h>
#include <stdbool.h>

#define SAFE_DISTANCE_MULTIPLIER 2 // multiplier for safe distance

#define THRESHOLD_DISTANCE 300

// Task declarations
void distanceMeasurementTask(void *pvParameters);
void obstacleDetectionTask(void *pvParameters);
bool calculateCollisionRisk(uint16_t measuredDistance);
int getCarSpeed(void);
// Shared variable and semaphore
volatile uint16_t frontMeasuredDistance = 0;
volatile uint16_t backMeasuredDistance = 0;
SemaphoreHandle_t xFrontDistanceSemaphore;
SemaphoreHandle_t xBackDistanceSemaphore;

// Event group for obstacle detection
EventGroupHandle_t xObstacleEventGroup;


void create_all_application_tasks(void) {
	// Initialize semaphores
	xFrontDistanceSemaphore = xSemaphoreCreateMutex();
	xBackDistanceSemaphore = xSemaphoreCreateMutex();

	// Initialize event group
	xObstacleEventGroup = xEventGroupCreate();

	// Create distance measurement tasks
	xTaskCreate(distanceMeasurementFrontTask, "FrontDistanceMeas", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(distanceMeasurementBackTask, "BackDistanceMeas", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

// Definitions for distanceMeasurementFrontTask, distanceMeasurementBackTask, obstacleEventHandlerTask...

void distanceMeasurementFrontTask(void *pvParameters) {
	
	printf("Distance measurement Front task\n");
	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)3 /*task id*/);
	#endif
	
	// Define the period of the task.
	const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Period of 1 second in ticks
	TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialise the xLastWakeTime variable with the current time.

	while (1) {
		uint16_t distance = 0;

		// Acquire the semaphore before reading the shared variable.
		if (xSemaphoreTake(xFrontDistanceSemaphore, portMAX_DELAY) == pdTRUE) {
			frontMeasuredDistance = hc_sr04_takeMeasurement(true);
			distance = frontMeasuredDistance;
			// Release the semaphore as soon as the shared variable is read.
			xSemaphoreGive(xFrontDistanceSemaphore);
		}

		// Calculate the collision risk.
		bool isHighRisk = calculateCollisionRisk(distance);
		if (isHighRisk) {
			printf("RISK!! RISK!!\n");
			// Take immediate action, such as notifying for risk.
			turnOnLight(true); // Indicate risk with the front light.
			} else {
			turnOffLight(true); // Turn off the front light if no risk.
		}

		// Wait for the next cycle.
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void distanceMeasurementBackTask(void *pvParameters) {
	
	printf("Distance measurement back task\n");
	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)1 /*task id*/);
	#endif
	
	// Define the period of the task.
	const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Period of 1 second in ticks
	TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialise the xLastWakeTime variable with the current time.

	while (1) {
		uint16_t distance = 0;

		// Acquire the semaphore before updating the shared variable.
		if (xSemaphoreTake(xBackDistanceSemaphore, portMAX_DELAY) == pdTRUE) {
			// Perform the distance measurement.
			backMeasuredDistance = hc_sr04_takeMeasurement(false);
			distance = backMeasuredDistance;
			printf("Back Measurement : %d\n", distance);
			// Release the semaphore.
			xSemaphoreGive(xBackDistanceSemaphore);
		}
		
		// Assess the collision risk.
		bool isHighRisk = calculateCollisionRisk(distance);
		if (isHighRisk) {
			printf("RISK!! RISK!!\n");
			// Take immediate action.
			turnOnLight(false); // Indicate risk with the back light.
			} else {
			// No risk detected.
			turnOffLight(false); // Turn off the back light.
		}
		
		// Wait for the next cycle.
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}


// Here i am getting random car speed 
int getCarSpeed() {
	// Assume cars' speed ranges from 30 to 100 units
	return (rand() % 71) + 30;
}

// This function calculates the risk of collision
bool calculateCollisionRisk(uint16_t measuredDistance) {
	int otherCarSpeed = getCarSpeed(); // Simulate other car's speed
	printf("\n Others speed : %d", otherCarSpeed);
	int ourCarSpeed = getCarSpeed(); // Get your car's current speed
	printf("our speed : %d", ourCarSpeed);

	// Calculate safe stopping distance (you would define the logic for this)
	int safeStoppingDistance = (ourCarSpeed + otherCarSpeed) * SAFE_DISTANCE_MULTIPLIER;
	printf("safe Stopping Distance : %d", safeStoppingDistance);
	printf(" measuredDistance  : %d", measuredDistance);

	// Determine if there is a high risk of collision
	bool isHighRisk = measuredDistance < safeStoppingDistance;

	return isHighRisk;
}


// Function to turn on the light
void turnOnLight(bool frontLight)
{
	
	uint8_t LIGHT_PIN = frontLight ? P_LIGHT_PIN_Front : P_LIGHT_PIN_Back; // Choose the correct pin based on the sensor
	// Set LIGHT_PIN high
	PORTC |= (1 << LIGHT_PIN);
}

// Function to turn off the light
void turnOffLight(bool frontLight)
{
	uint8_t LIGHT_PIN = frontLight ? P_LIGHT_PIN_Front : P_LIGHT_PIN_Back; // Choose the correct pin based on the sensor

	// Set LIGHT_PIN low
	PORTC &= ~(1 << LIGHT_PIN);
}