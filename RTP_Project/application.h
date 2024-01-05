/*
 * Application.h
 *
 * Created: 24/12/2023 09.46.58
 *  Author: himal
 */ 
#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>
#include "hc_sr04.h"
#include "avr/interrupt.h"
// #define LIGHT_PIN PC0 // Define the pin for the light
#define P_LIGHT_PIN_Front PC0
#define P_LIGHT_PIN_Back PC6

void create_all_application_tasks(void);

void distanceMeasurementFrontTask(void *pvParameters);
void distanceMeasurementBackTask(void *pvParameters);
void obstacleDetectionTask(void *pvParameters);
void obstacleDetectionForBackTask(void *pvParameters);

// Function to turn on/off the light
void turnOnLight(bool frontLigth);
void turnOffLight(bool frontLigth);
