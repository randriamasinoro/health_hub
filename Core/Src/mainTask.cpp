/**
  ******************************************************************************
  * @file           : mainTask.cpp
  * @brief          : Main program wrapper to cpp code body
  ******************************************************************************
*/

#include "main.h"
#include "mainTask.hpp"

#include <math.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "Protocentral_MAX30205.h"
//Here the Si7021 if you have not the fever click
//Uncomment following lines if you're using Si7021
//#include "Adafruit_Si7021.h"

#include "MAX30105.h"
#include "heartRate.h"

#include <U8g2lib.h>

// Get variables declared in main.c
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;

static MAX30205 tempSensor;
static U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D3, /* reset=*/ U8X8_PIN_NONE);
static MAX30105 particleSensor;
static Adafruit_MPU6050 mpu;
//MAX30105 particleSensor;
//Adafruit_MPU6050 mpu;
static void initializeMPU6050(Adafruit_MPU6050* mpu)
{
	// Try to initialize!
		  if (!mpu->begin()) {
		    while (1) {
		    	delay(10);
		    }
		  }

		  mpu->setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
		  mpu->setAccelerometerRange(MPU6050_RANGE_2_G);
		  mpu->setMotionDetectionThreshold(1);
		  mpu->setMotionDetectionDuration(20);
		  mpu->setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
		  mpu->setInterruptPinPolarity(true);
		  mpu->setMotionInterrupt(true);

		  delay(100);
}


static void initializeMAX30205(MAX30205* tempSensor)
{
	WIRE.begin();

	//scan for temperature in every 30s until a sensor is found. Scan for both addresses 0x48 and 0x49
	while(!tempSensor->scanAvailableSensors()){
	  delay(30000);
	}

	tempSensor->begin();   // set continuous mode, active mode
}

static void initializeMAX30105(MAX30105* particleSensor)
{
	particleSensor->begin(Wire, I2C_SPEED_FAST);
	particleSensor->setup(); //Configure sensor with default settings
	particleSensor->setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
	particleSensor->setPulseAmplitudeGreen(0); //Turn off Green LED
}

static void initializeGrayScreen(U8G2* u8g2)
{
	u8g2->begin();
	u8g2->enableUTF8Print();
}

//Uncomment following lines if you're using Si7021
//static void initializeSi7021(Adafruit_Si7021* sensor)
//{
//	if (!sensor->begin())
//	{
//		while (1)
//		{
//			delay(10);
//		}
//	}
//}

void initTemp()
{
	initializeMAX30205(&tempSensor);

}

void initBpm()
{
	initializeMAX30105(&particleSensor);
}

void initMPU(){
	initializeMPU6050(&mpu);
}
void initDisplay()
{
	initializeGrayScreen(&u8g2);
}
void display(float temperature, int beat, float accel, float gyro){

	  //print data some data on screen
	  u8g2.setFont(u8g2_font_b10_t_japanese1);  // all the glyphs of "こんにちは世界" are already included in japanese1: Lerning Level 1-6
	  u8g2.setFontDirection(0);
	  u8g2.firstPage();
	  do {
		u8g2.setCursor(0, 15);
		u8g2.print("Accel-Z: ");
	  	u8g2.print(accel);
	  	u8g2.setCursor(0, 30);
	  	u8g2.print("gyro-norm: ");
	  	u8g2.print(gyro);
	  	u8g2.setCursor(0, 45);
	  	u8g2.print("Temp: ");
	  	u8g2.print(temperature);
		u8g2.setCursor(0, 60);
		u8g2.print("BPM: ");
	  	u8g2.print(beat);
	} while ( u8g2.nextPage() );
}


float getTemp(){
	float feverTemperature = tempSensor.getTemperature();
	return feverTemperature;
}

long acquireHeartSample()
{
    long irValue = particleSensor.getIR();
    return irValue;
}

void getAccelGyro(sensor_vec3_t * accel, sensor_vec3_t * gyro)
{
	sensors_event_t a, g, temp;
    if (mpu.getMotionInterruptStatus()) {
        mpu.getEvent(&a, &g, &temp);
        accel->x = a.acceleration.x;
        accel->y = a.acceleration.y;
        accel->z = a.acceleration.z;

        gyro->x = g.gyro.x;
        gyro->y = g.gyro.y;
        gyro->z = g.gyro.z;
    }
    // sinon ne fait rien
}

int calculateBPM(long irValue)
{
    static const byte RATE_SIZE = 4;
    static byte rates[RATE_SIZE] = {0};
    static byte rateSpot = 0;
    static long lastBeat = 0;
    static int beatAvg = 0;

    float beatsPerMinute;

    // Détection du doigt absent
    if (irValue < 8000)
    {
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
            rates[x] = 0;
        rateSpot = 0;
        lastBeat = 0;
        return 0;
    }

    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;

            // Calculer la moyenne sur les valeurs valides
            beatAvg = 0;
            int validCount = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
            {
                if (rates[x] > 0)
                {
                    beatAvg += rates[x];
                    validCount++;
                }
            }
            if (validCount > 0)
                beatAvg /= validCount;

        }
    }

    return beatAvg;
}

imu_norm_t calculateNorms(imu_data_t *data) {
    imu_norm_t norm;

    // Norme du vecteur accélération
    norm.accel_norm = sqrtf(
        data->accel.x - data->accel.x +
        data->accel.y - data->accel.y +
        data->accel.z * data->accel.z
    );

    // Norme du vecteur gyroscope
    norm.gyro_norm = sqrtf(
        data->gyro.x * data->gyro.x +
        data->gyro.y * data->gyro.y +
        data->gyro.z * data->gyro.z
    );

    return norm;
}


// All C calls from main.c below
extern "C"
{
	void initDisplayC()
	{
		initDisplay();
	}
	void initBpmC(){
		 initBpm();
	}

	void initTempC()
	{
		initTemp();
	}

	void initMPUC()
	{
		initMPU();
	}

	void getAccelGyroC(sensor_vec3_t* accel, sensor_vec3_t* gyro)
	{
		getAccelGyro(accel, gyro);
	}
	float getTempC()
	{
		return getTemp();
	}

	void displayC(float temperature, int beat, float accel, float gyro)
	{
		display(temperature,beat,accel,gyro);
	}
    long acquireHeartSampleC()
    {
        return acquireHeartSample();
    }

    int calculateBPMC(long irValue)
    {
        return calculateBPM(irValue);
    }

    imu_norm_t calculateNormsC(imu_data_t *data){
    	return calculateNorms(data);
    }

}

