/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mainTask.hpp"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* Definitions for MotionTask */
osThreadId_t MotionTaskHandle;
const osThreadAttr_t MotionTask_attributes = {
  .name = "MotionTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for HeartRateTask */
osThreadId_t HeartRateTaskHandle;
const osThreadAttr_t HeartRateTask_attributes = {
  .name = "HeartRateTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for processTaskBPM */
osThreadId_t processTaskBPMHandle;
const osThreadAttr_t processTaskBPM_attributes = {
  .name = "processTaskBPM",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TempTask */
osThreadId_t TempTaskHandle;
const osThreadAttr_t TempTask_attributes = {
  .name = "TempTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for AttitudeTask */
osThreadId_t AttitudeTaskHandle;
const osThreadAttr_t AttitudeTask_attributes = {
  .name = "AttitudeTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for qBPMDisplay */
osMessageQueueId_t qBPMDisplayHandle;
const osMessageQueueAttr_t qBPMDisplay_attributes = {
  .name = "qBPMDisplay"
};
/* Definitions for qTempData */
osMessageQueueId_t qTempDataHandle;
const osMessageQueueAttr_t qTempData_attributes = {
  .name = "qTempData"
};
/* Definitions for qAttitudeDisplay */
osMessageQueueId_t qAttitudeDisplayHandle;
const osMessageQueueAttr_t qAttitudeDisplay_attributes = {
  .name = "qAttitudeDisplay"
};
/* Definitions for qAttitudeAlgo */
osMessageQueueId_t qAttitudeAlgoHandle;
const osMessageQueueAttr_t qAttitudeAlgo_attributes = {
  .name = "qAttitudeAlgo"
};
/* Definitions for qBPMalgo */
osMessageQueueId_t qBPMalgoHandle;
const osMessageQueueAttr_t qBPMalgo_attributes = {
  .name = "qBPMalgo"
};
/* Definitions for mutexI2C */
osMutexId_t mutexI2CHandle;
const osMutexAttr_t mutexI2C_attributes = {
  .name = "mutexI2C"
};
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM11_Init(void);
void fnMotionTask(void *argument);
void fnHeartTask(void *argument);
void fnBPM(void *argument);
void fnTempTask(void *argument);
void fnDisplayTask(void *argument);
void fnAttitudeTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  initMPUC();
  initBpmC();
  initTempC();
  initDisplayC();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of mutexI2C */
  mutexI2CHandle = osMutexNew(&mutexI2C_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qBPMDisplay */
  qBPMDisplayHandle = osMessageQueueNew (1, 4, &qBPMDisplay_attributes);

  /* creation of qTempData */
  qTempDataHandle = osMessageQueueNew (1, 4, &qTempData_attributes);

  /* creation of qAttitudeDisplay */
  qAttitudeDisplayHandle = osMessageQueueNew (1, 8, &qAttitudeDisplay_attributes);

  /* creation of qAttitudeAlgo */
  qAttitudeAlgoHandle = osMessageQueueNew (1, 24, &qAttitudeAlgo_attributes);

  /* creation of qBPMalgo */
  qBPMalgoHandle = osMessageQueueNew (10, sizeof(uint32_t), &qBPMalgo_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MotionTask */
  MotionTaskHandle = osThreadNew(fnMotionTask, NULL, &MotionTask_attributes);

  /* creation of HeartRateTask */
  HeartRateTaskHandle = osThreadNew(fnHeartTask, NULL, &HeartRateTask_attributes);

  /* creation of processTaskBPM */
  processTaskBPMHandle = osThreadNew(fnBPM, NULL, &processTaskBPM_attributes);

  /* creation of TempTask */
  TempTaskHandle = osThreadNew(fnTempTask, NULL, &TempTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(fnDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of AttitudeTask */
  AttitudeTaskHandle = osThreadNew(fnAttitudeTask, NULL, &AttitudeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //Never reached
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */
}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 0;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 65535;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_fnMotionTask */
/**
  * @brief  Function implementing the MotionTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_fnMotionTask */
void fnMotionTask(void *argument)
{
  /* USER CODE BEGIN 5 */
	sensor_vec3_t a, g;
	imu_data_t data;
  /* Infinite loop */
  for(;;)
  {	osMutexAcquire(mutexI2CHandle, osWaitForever);
	 getAccelGyroC(&a, &g);
	 osMutexRelease(mutexI2CHandle);
	 data.accel = a;
	 data.gyro  =g;
osMessageQueuePut(qAttitudeAlgoHandle, &data, 0, 0);

    osDelay(50);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_fnHeartTask */
/**
* @brief Function implementing the HeartRateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fnHeartTask */
void fnHeartTask(void *argument)
{
  /* USER CODE BEGIN fnHeartTask */
	//osMutexAcquire(mutexI2CHandle, osWaitForever);

	//osMutexRelease(mutexI2CHandle);
	long irSample;
  /* Infinite loop */
  for(;;)
  {
	  osMutexAcquire(mutexI2CHandle, osWaitForever);
	  	irSample = acquireHeartSampleC();
	  	osMutexRelease(mutexI2CHandle);
		// Envoyer l'échantillon à la task de calcul
		osMessageQueuePut(qBPMalgoHandle, &irSample, 0, 10);
    osDelay(10);
  }
  /* USER CODE END fnHeartTask */
}

/* USER CODE BEGIN Header_fnBPM */
/**
* @brief Function implementing the processTaskBPM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fnBPM */
void fnBPM(void *argument)
{
  /* USER CODE BEGIN fnBPM */
	long irSample;
	int beat;
  /* Infinite loop */
  for(;;)
  {
	  if (osMessageQueueGet(qBPMalgoHandle, &irSample, NULL, osWaitForever) == osOK)
	  	{
	  		// Calculer le BPM à partir de l'échantillon
	  		beat = calculateBPMC(irSample);

	  		// Envoyer le résultat à l'affichage
	  		osMessageQueuePut(qBPMDisplayHandle, &beat, 0, 10);
	  	}
	  osDelay(10);
	    }
  /* USER CODE END fnBPM */
}

/* USER CODE BEGIN Header_fnTempTask */
/**
* @brief Function implementing the TempTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fnTempTask */
void fnTempTask(void *argument)
{
  /* USER CODE BEGIN fnTempTask */
	//osMutexAcquire(mutexI2CHandle, osWaitForever);

	//osMutexRelease(mutexI2CHandle);
	float temp = 00.00f;
	//uint8_t buf[32]="je suis task temp\r\n";
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osMutexAcquire(mutexI2CHandle, osWaitForever);
		temp = getTempC();
		osMutexRelease(mutexI2CHandle);

	  osMessageQueuePut(qTempDataHandle, &temp, 0, 0);

	          // envoyer sur UART2
	osDelay(1000);
  }
  /* USER CODE END fnTempTask */
}

/* USER CODE BEGIN Header_fnDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fnDisplayTask */
void fnDisplayTask(void *argument)
{
  /* USER CODE BEGIN fnDisplayTask */
	float temp = 22.00f;
	int beat;
	imu_norm_t norm;
	float accel_norm;
	float gyro_norm;
  /* Infinite loop */
  for(;;)
  {
	  osMessageQueueGet(qTempDataHandle, &temp, NULL, 0);
	  osMessageQueueGet(qBPMDisplayHandle, &beat, NULL, 0);
	  osMessageQueueGet(qAttitudeDisplayHandle, &norm, NULL, 0);
	  accel_norm = norm.accel_norm;
	  gyro_norm = norm.gyro_norm;
	  displayC(temp, beat,accel_norm,gyro_norm);
    osDelay(100);
  }
  /* USER CODE END fnDisplayTask */
}

/* USER CODE BEGIN Header_fnAttitudeTask */
/**
* @brief Function implementing the AttitudeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fnAttitudeTask */
void fnAttitudeTask(void *argument)
{
  /* USER CODE BEGIN fnAttitudeTask */
	imu_data_t data;
	imu_norm_t norm;
  /* Infinite loop */
  for(;;)
  {
	  osMessageQueueGet(qAttitudeAlgoHandle, &data, NULL, 0);
	  norm = calculateNormsC(&data);
	  osMessageQueuePut(qAttitudeDisplayHandle, &norm, 0, 0);
    osDelay(100);
  }
  /* USER CODE END fnAttitudeTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
