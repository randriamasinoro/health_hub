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

#include <stdint.h>
#include <stdlib.h>


void mainTaskCpp();	// Cpp function to call into main event loop


#ifdef __cplusplus
extern "C"
{
#endif

	void mainTaskC();

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
