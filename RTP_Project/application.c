#include "application.h"

// Task declarations
void distanceMeasurementTask(void *pvParameters);
void obstacleDetectionTask(void *pvParameters);

// Shared variable and semaphore
volatile uint16_t measuredDistance = 0;
SemaphoreHandle_t xDistanceSemaphore;


void create_all_application_tasks(void)
{
	
	xDistanceSemaphore = xSemaphoreCreateMutex();  // Initialize semaphore

	// Create tasks
	xTaskCreate(
	distanceMeasurementTask,
	"DistanceMeas",
	configMINIMAL_STACK_SIZE,
	NULL,
	3, // Priority
	NULL);

	xTaskCreate(
	obstacleDetectionTask,
	"ObstacleDetect",
	configMINIMAL_STACK_SIZE,
	NULL,
	1, // Priority
	NULL);
}

// Implementations of distanceMeasurementTask and obstacleDetectionTask...

void distanceMeasurementTask(void *pvParameters)
{
	printf("Distance measurnment task ");
	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)4 /*task id*/);
	#endif

	const TickType_t xDelay = 500 / portTICK_PERIOD_MS; // Delay for 500 ms

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		uint16_t distance = hc_sr04_takeMeasurement();

		// Acquire the semaphore before updating the shared variable
		if (xSemaphoreTake(xDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			measuredDistance = distance;
			xSemaphoreGive(xDistanceSemaphore); // Release the semaphore
		}

		vTaskDelay(xDelay); // Wait for the next cycle
	}
}

void obstacleDetectionTask(void *pvParameters)
{
			printf("Obstackele detection task");

	// In every task before loop:
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set taskTag
	vTaskSetApplicationTaskTag(NULL, (void *)2 /*task id*/);
	#endif

	uint16_t distance;

	while (1)
	{
		// Acquire the semaphore before reading the shared variable
		if (xSemaphoreTake(xDistanceSemaphore, portMAX_DELAY) == pdTRUE)
		{
			distance = measuredDistance;
			xSemaphoreGive(xDistanceSemaphore); // Release the semaphore
		}
		// Check if distance is below a certain threshold
		if (distance < 500) // Assuming 20 cm as a constraint
		{
			printf("Obstacle too close! ,%d \n" , distance);
			turnOnLight(); // Turn on the light

		}
		else{
			printf("Obstacle is far away from us ,%d \n" , distance);
			turnOffLight();
		}

		
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

// Function to turn on the light
void turnOnLight(void)
{
	// Set LIGHT_PIN high
	PORTC |= (1 << LIGHT_PIN);
}

// Function to turn off the light
void turnOffLight(void)
{
	// Set LIGHT_PIN low
	PORTC &= ~(1 << LIGHT_PIN);
}