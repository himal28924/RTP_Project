/*
 * ultraSonicSensor.h
 *
 * Created: 24/12/2023 10.00.02
 *  Author: himal
 */

 #pragma once
 
 void create_ultraSonic_sensor_task(UBaseType_t priority);
 void distanceMeasure();
 uint16_t distanceMeasure_getMM();
 

