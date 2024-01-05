#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>
#include "hc_sr04.h"
#include "avr/interrupt.h"
void initializeSystem(void);
#include "application.h"

int main(void)
{
	initializeSystem();  // Initialize system configurations

	create_all_application_tasks();  // Create all tasks from application.c
	vTaskStartScheduler();  // Start the scheduler


	for(;;);
	return 0;
}

void initializeSystem(void)
{
	trace_init();
	stdio_initialise(ser_USART0);
	status_leds_initialise(5);
	DDRC |= (1 << P_LIGHT_PIN_Front);
	PORTC &= ~(1 << P_LIGHT_PIN_Front);
	DDRC |= (1 << P_LIGHT_PIN_Back);
	PORTC &= ~(1 << P_LIGHT_PIN_Back);
	hc_sr04_init();

	printf("System Initialization Complete\n");
}
