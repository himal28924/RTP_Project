#include "ultrasonicsensor.h"
#include "hc_sr04.h"

// Global variables for sensor distances
volatile uint16_t frontDistance = 0;
volatile uint16_t backDistance = 0;

// Semaphores for thread-safe access
SemaphoreHandle_t xFrontDistanceSemaphore;
SemaphoreHandle_t xBackDistanceSemaphore;

void initUltrasonicSensors(void) {
	// Initialize hardware pins for front and back sensors
	// Example: set pin modes for trigger and echo pins

	// Initialize semaphores
	xFrontDistanceSemaphore = xSemaphoreCreateMutex();
	xBackDistanceSemaphore = xSemaphoreCreateMutex();
}

uint16_t measureFrontDistance(void) {
	// Code to measure front distance
	uint16_t frontDistanceMeasured = hc_sr04_takeMeasurement(FRONT_SENSOR_TRIGGER_PIN, FRONT_SENSOR_ECHO_PIN);
	return frontDistanceMeasured;
}

uint16_t measureBackDistance(void) {
	// Code to measure back distance
	uint16_t backDistanceMeasured = hc_sr04_takeMeasurement(BACK_SENSOR_TRIGGER_PIN, BACK_SENSOR_ECHO_PIN);
	return backDistanceMeasured;
}

void frontSensorTask(void *pvParameters) {
	while (1) {
		uint16_t distance = measureFrontDistance();

		// Update global variable safely
		if (xSemaphoreTake(xFrontDistanceSemaphore, portMAX_DELAY) == pdTRUE) {
			frontDistance = distance;
			xSemaphoreGive(xFrontDistanceSemaphore);
		}

		vTaskDelay(pdMS_TO_TICKS(500)); // Delay as required
	}
}

void backSensorTask(void *pvParameters) {
	while (1) {
		uint16_t distance = measureBackDistance();

		// Update global variable safely
		if (xSemaphoreTake(xBackDistanceSemaphore, portMAX_DELAY) == pdTRUE) {
			backDistance = distance;
			xSemaphoreGive(xBackDistanceSemaphore);
		}

		vTaskDelay(pdMS_TO_TICKS(500)); // Delay as required
	}
}
