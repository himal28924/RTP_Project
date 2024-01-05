#include "application.h"

// Task declarations
void distanceMeasurementTask(void *pvParameters);
void obstacleDetectionTask(void *pvParameters);

// Shared variable and semaphore
volatile uint16_t frontMeasuredDistance = 0;
volatile uint16_t backMeasuredDistance = 0;
SemaphoreHandle_t xFrontDistanceSemaphore;
SemaphoreHandle_t xBackDistanceSemaphore;


void create_all_application_tasks(void)
{
	
    // Initialize semaphores
    xFrontDistanceSemaphore = xSemaphoreCreateMutex();
    xBackDistanceSemaphore = xSemaphoreCreateMutex();

    // Create front distance measurement task
    xTaskCreate(
    distanceMeasurementFrontTask,
    "FrontDistanceMeas",
    configMINIMAL_STACK_SIZE,
    NULL,
    3, // Priority
    NULL);

    // Create back distance measurement task
    xTaskCreate(
    distanceMeasurementBackTask,
    "BackDistanceMeas",
    configMINIMAL_STACK_SIZE,
    NULL,
    3, // Priority
    NULL);

    // Create obstacle detection task
    xTaskCreate(
    obstacleDetectionTask,
    "ObstacleDetect",
    configMINIMAL_STACK_SIZE,
    NULL,
    1, // Priority
    NULL);
	
	
	// Create obstacle detection task
	xTaskCreate(
	obstacleDetectionForBackTask,
	"obstacleDetectionForBack",
	configMINIMAL_STACK_SIZE,
	NULL,
	1, // Priority
	NULL);
}

// Implementations of distanceMeasurementTask and obstacleDetectionTask...

void distanceMeasurementFrontTask(void *pvParameters)
{
	printf("Distance measurement task ");
	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)5 /*task id*/);
	#endif

	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS; // Delay for 500 ms

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		uint16_t distance = hc_sr04_takeMeasurement(true);

		// Acquire the semaphore before updating the shared variable
		if (xSemaphoreTake(xFrontDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			frontMeasuredDistance = distance;
			xSemaphoreGive(xFrontDistanceSemaphore); // Release the semaphore
		}

		vTaskDelay(xDelay); // Wait for the next cycle
	}
}

void obstacleDetectionTask(void *pvParameters)
{
			printf("Obstacle detection task");

	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)1 /*task id*/);
	#endif

	uint16_t distance;

	while (1)
	{
		// Acquire the semaphore before reading the shared variable
		if (xSemaphoreTake(xFrontDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			distance = frontMeasuredDistance;
			xSemaphoreGive(xFrontDistanceSemaphore); // Release the semaphore
		}
		// Check if distance is below a certain threshold
		if (distance < 500) // Assuming 20 cm as a constraint
		{
			printf("Obstacle too close! ,%d \n" , distance);
			turnOnLight(true); // Turn on the light

		}
		else{
			printf("Obstacle is far away from us ,%d \n" , distance);
			turnOffLight(true);
		}

		
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

void distanceMeasurementBackTask(void *pvParameters)
{
	printf("Distance measurement back task ");
	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)3 /*task id*/);
	#endif

	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS; // Delay for 500 ms

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		uint16_t distance = hc_sr04_takeMeasurement(false);

		// Acquire the semaphore before updating the shared variable
		if (xSemaphoreTake(xBackDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			backMeasuredDistance = distance;
			printf("Measurement!! ,%d \n" , distance);
			xSemaphoreGive(xBackDistanceSemaphore); // Release the semaphore
		}

		vTaskDelay(xDelay); // Wait for the next cycle
	}
}

void obstacleDetectionForBackTask(void *pvParameters)
{
	printf("Obstacle detection task");

	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)4 /*task id*/);
	#endif

	uint16_t distance;

	while (1)
	{
		// Acquire the semaphore before reading the shared variable
		if (xSemaphoreTake(xFrontDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			distance = backMeasuredDistance;
			xSemaphoreGive(xFrontDistanceSemaphore); // Release the semaphore
		}
		// Check if distance is below a certain threshold
		if (distance < 500) // Assuming 20 cm as a constraint
		{
			printf("Obstacle too close! ,%d \n" , distance);
			turnOnLight(false); // Turn on the light

		}
		else{
			printf("Obstacle is far away from us ,%d \n" , distance);
			turnOffLight(false);
		}

		
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
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


