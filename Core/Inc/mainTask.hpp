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

typedef struct {
    float x;
    float y;
    float z;
} sensor_vec3_t;

typedef struct {
    sensor_vec3_t accel;
    sensor_vec3_t gyro;
} imu_data_t;

typedef struct {
    float accel_norm;
    float gyro_norm;
} imu_norm_t;

void initDisplay();
void display(float temperature, int beat, float accel, float gyro);
void initMPU();
void initBpm();
void initTemp();// Cpp function to call into main event loop
float getTemp();
void getAccelGyro(sensor_vec3_t* accel, sensor_vec3_t* gyro);
int calculateBPM(long irValue);
long acquireHeartSample();
imu_norm_t calculateIMUNorms(imu_data_t* data);
#ifdef __cplusplus
extern "C"
{
#endif

	void initDisplayC();
	void getAccelGyroC(sensor_vec3_t* accel, sensor_vec3_t* gyro);
	void initMPUC();
	long acquireHeartSampleC();
	int calculateBPMC(long irValue);
	void initTempC();
	float getTempC();
	void displayC(float temperature, int beat, float accel, float gyro);
	void initBpmC();
	imu_norm_t calculateNormsC(imu_data_t *data);
#ifdef __cplusplus
}

#endif

#endif /* __MAIN_H */
