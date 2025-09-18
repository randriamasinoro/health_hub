/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : mainTask.h
  * @brief          : Header for mainTask.cpp file.
  *                   This file contains the wrapper between C and Cpp code
  ******************************************************************************
  */



/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_TASK_HPP
#define __MAIN_TASK_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "Protocentral_MAX30205.h"

#include "MAX30105.h"
#include "heartRate.h"

#include <stdint.h>
#include <stdlib.h>
#include <U8g2lib.h>


void mainTaskCpp(void const * argument);	// Cpp function to call into main event loop


#ifdef __cplusplus
extern "C"
{
#endif

	void mainTaskC(void const * argument);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
