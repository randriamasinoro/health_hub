/**
  ******************************************************************************
  * @file           : mainTask.cpp
  * @brief          : Main program wrapper to cpp code body
  ******************************************************************************
*/

#include "main.h"
#include "mainTask.hpp"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "Protocentral_MAX30205.h"

#include "MAX30105.h"
#include "heartRate.h"

#include <U8g2lib.h>

// Get variables declared in main.c
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

extern UART_HandleTypeDef huart1;

static void initializeMPU6050(Adafruit_MPU6050* mpu)
{
	// Try to initialize!
		  if (!mpu->begin()) {
		    while (1) {
		    	delay(10);
		    }
		  }

		  mpu->setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
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

static void initializeGrayScreen(U8G2_SH1107_SEEED_128X128_1_SW_I2C* u8g2)
{
	u8g2->begin();
	u8g2->enableUTF8Print();
}

void mainTaskCpp()
{
	//Initialize MPU6050 (I2C1)
	Adafruit_MPU6050 mpu;
	initializeMPU6050(&mpu);

	//Initialize MAX30205 (I2C1)
	MAX30205 tempSensor;
	initializeMAX30205(&tempSensor);

	//Initialize MAX30105 (I2C1)
	MAX30105 particleSensor;
	initializeMAX30105(&particleSensor);

	//Initialize Screen
	U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D3, /* reset=*/ U8X8_PIN_NONE);
	initializeGrayScreen(&u8g2);

	//MAX30105 variables
	const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
	byte rates[RATE_SIZE]; //Array of heart rates
	byte rateSpot = 0;
	long lastBeat = 0; //Time at which the last beat occurred
	float beatsPerMinute;
	int beatAvg;

  /* Infinite loop */
  for(;;)
  {
	//Get sensor (MPU6050) values
	sensors_event_t a, g, temp;
	if(mpu.getMotionInterruptStatus()) {
	    /* Get new sensor events with the readings */
	    mpu.getEvent(&a, &g, &temp);
	  }

	//Get sensor (MAX30205) temperature
	float feverTemperature = tempSensor.getTemperature();

	//Get sensor (MAX30105) IR value & compute heart beat
	long irValue = particleSensor.getIR();

	  if (checkForBeat(irValue) == true)
	  {
	    //We sensed a beat!
	    long delta = millis() - lastBeat;
	    lastBeat = millis();

	    beatsPerMinute = 60 / (delta / 1000.0);

	    if (beatsPerMinute < 255 && beatsPerMinute > 20)
	    {
	      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
	      rateSpot %= RATE_SIZE; //Wrap variable

	      //Take average of readings
	      beatAvg = 0;
	      for (byte x = 0 ; x < RATE_SIZE ; x++)
	        beatAvg += rates[x];
	      beatAvg /= RATE_SIZE;
	    }
	  }
	  
	  //Send here something through to Serial?

	  //print data some data on screen
	  u8g2.setFont(u8g2_font_b10_t_japanese1);  // all the glyphs of "こんにちは世界" are already included in japanese1: Lerning Level 1-6
	  u8g2.setFontDirection(0);
	  u8g2.firstPage();
	  do {
		u8g2.setCursor(0, 15);
	  	u8g2.print("こんにちは世界");
	  	u8g2.setCursor(0, 30);
	  	u8g2.print("Hello Word");
	  	u8g2.setCursor(0, 45);
	  	u8g2.print("Bonjour le monde");
		u8g2.setCursor(0, 60);
	  	u8g2.print(feverTemperature);
	  	u8g2.setCursor(0, 75);
	  	u8g2.print("etc...");
	} while ( u8g2.nextPage() );

	// Toggle User led (LD2)
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	delay(500);
  }
}

// All C calls from main.c below
extern "C"
{
	void mainTaskC()
	{
		mainTaskCpp();
	}
}
