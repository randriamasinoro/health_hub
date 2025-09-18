/**
  ******************************************************************************
  * @file           : mainTask.cpp
  * @brief          : Main program wrapper to cpp code body
  ******************************************************************************
*/

#include "main.h"
#include "cmsis_os.h"
#include "mainTask.hpp"

void mainTaskCpp(void const * argument)
{
	//Initialize MPU6050
	Adafruit_MPU6050 mpu;

	  // Try to initialize!
	  if (!mpu.begin()) {
	    while (1) {
	    	osDelay(10);
	    }
	  }

	  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
	  mpu.setMotionDetectionThreshold(1);
	  mpu.setMotionDetectionDuration(20);
	  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
	  mpu.setInterruptPinPolarity(true);
	  mpu.setMotionInterrupt(true);

	  osDelay(100);

	  //Initialize MAX30205
	  MAX30205 tempSensor;

	  WIRE.begin();

	  //scan for temperature in every 30s until a sensor is found. Scan for both addresses 0x48 and 0x49
	  while(!tempSensor.scanAvailableSensors()){
		  delay(30000);
	  }

	  tempSensor.begin();   // set continuous mode, active mode

	  //Initialize MAX30105
	  MAX30105 particleSensor;

	  const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
	  byte rates[RATE_SIZE]; //Array of heart rates
	  byte rateSpot = 0;
	  long lastBeat = 0; //Time at which the last beat occurred

	  float beatsPerMinute;
	  int beatAvg;

	  particleSensor.begin(Wire, I2C_SPEED_FAST);

	  particleSensor.setup(); //Configure sensor with default settings
	  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
	  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

	  //Initialize Screen
	  U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D3, /* reset=*/ U8X8_PIN_NONE);

	  u8g2.begin();
	  u8g2.enableUTF8Print();

  /* Infinite loop */
  for(;;)
  {
	  sensors_event_t a, g, temp;
	  if(mpu.getMotionInterruptStatus()) {
	      /* Get new sensor events with the readings */
	      mpu.getEvent(&a, &g, &temp);
	    }

	  osDelay(10);

	  float feverTemperature = tempSensor.getTemperature(); // read temperature for every 100ms

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

	    u8g2.setFont(u8g2_font_b10_t_japanese1);  // all the glyphs of "こんにちは世界" are already included in japanese1: Lerning Level 1-6
	      u8g2.setFontDirection(0);
	      u8g2.firstPage();
	      do {
	    	  u8g2.setCursor(0, 15);
	    	  	      u8g2.print("Acceleration:");

	    	  	      u8g2.setCursor(70, 15);
	    	  	      u8g2.print("X:");
	    	  	      u8g2.print(a.acceleration.x);
	    	  	      u8g2.setCursor(70, 25);
	    	  	      u8g2.print("Y:");
	    	  	      u8g2.print(a.acceleration.y);
	    	  	      u8g2.setCursor(70, 35);
	    	  	      u8g2.print("Z:");
	    	  	      u8g2.print(a.acceleration.z);

	    	  	      u8g2.setCursor(0, 50);
	    	  	      u8g2.print("Gyroscope:");
	    	  	      u8g2.setCursor(70, 50);
	    	  	      u8g2.print("X:");
	    	  	      u8g2.print(g.gyro.x);
	    	  	      u8g2.setCursor(70, 60);
	    	  	      u8g2.print("Y:");
	    	  	      u8g2.print(g.gyro.y);
	    	  	      u8g2.setCursor(70, 70);
	    	  	      u8g2.print("Z:");
	    	  	      u8g2.print(g.gyro.z);

	    	  	      u8g2.setCursor(0, 85);
	    	  	      u8g2.print("Room Temp:");
	    	  	      u8g2.setCursor(70, 85);
	    	  	      u8g2.print(temp.temperature);
	    	  	      u8g2.print("°C");

	    	  	      // Print MAX30105 Data
	    	  	      u8g2.setCursor(0, 100);
	    	  	      u8g2.print("Pulse:");
	    	  	      u8g2.setCursor(70, 100);
	    	  	      u8g2.print("IR:");
	    	  	      u8g2.print(irValue);
	    	  	      u8g2.setCursor(70, 110);
	    	  	      u8g2.print("BPM:");
	    	  	      u8g2.print(beatsPerMinute);

	    	  	      //print MAX30205 Temperature (Fever)
	    	  	      u8g2.setCursor(0, 125);
	    	  	      u8g2.print("Fever °C:");
	    	  	      u8g2.setCursor(70, 125);
	    	  	      u8g2.print(feverTemperature);
	      } while ( u8g2.nextPage() );

	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  osDelay(500);
  }
}

// All C calls from main.c below
extern "C"
{
	void mainTaskC(void const * argument)
	{
		mainTaskCpp(argument);
	}
}
