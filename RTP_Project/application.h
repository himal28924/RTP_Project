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
#include "event_groups.h"

// #define LIGHT_PIN PC0 // Define the pin for the light
#define P_LIGHT_PIN_Front PC0
#define P_LIGHT_PIN_Back PC6

#define FRONT_OBSTACLE_BIT (1 << 0)
#define BACK_OBSTACLE_BIT  (1 << 1)
extern EventGroupHandle_t xObstacleEventGroup;

// Define your constants, include guards, and other includes...

void create_all_application_tasks(void);
void distanceMeasurementFrontTask(void *pvParameters);
void distanceMeasurementBackTask(void *pvParameters);
void obstacleDetectionFrontTask(void *pvParameters);
void obstacleDetectionBackTask(void *pvParameters);
void turnOnLight(bool isFrontLight);
void turnOffLight(bool isFrontLight);


void turnOnLight(bool frontLight);
void turnOffLight(bool frontLight);
