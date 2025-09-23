# Health Hub

The objective is to capture some Health data and send them to [Thingsboard](https://thingsboard.io/) through UART.

To send the data to Thingsboard check the [Health_Hub-Thingsboard-Transfer](Health_Hub-Thingsboard-Transfer/README.md) folder.

## Material

This project was made for following boards:

| Number | Functions | Name |
|---|---|---|
|1| Processing | STM32F411RE|
|1| Fever temperature | Fever click/MAX30205|
|1| smoke detector, heart rate and pulse oximeter sensor |MAX30105|
|1| 6 DoF MEMS (accelerometer + gyroscope) and a temperature sensor| MPU6050|
|1|Gray screen| OLED Display 1.12 inch v2.0|

DoF: Degree of Freedom.
MEMS: Micro-ElectroMechanical Systems.

Additionally, for connections we shall have:
 - Jumper wires,
 - 1 USB/Serial (TTL) cable,
 - 1 cable mini USB

## Librairies usage

This code was developed with STM32Cube IDE with Some Arduino wrapper.

Thanks to that wrapper it is possible to use some Arduino like code.
The examples provided (.ino files), can be used the same way in your main application.

(!) The wrapper was not used entirely & have some modification in code (due to partial use). Some arduino function (like `Serial.print`) may not work, use HAL STM32Cube functions instead.

Here the included library list (no need further installation):

| Name | GitHub link |
|---|---|
| Arduino core support for STM32 based boards (Wrapper) | https://github.com/stm32duino/Arduino_Core_STM32 |
| SparkFun MAX301x Particle Sensor Library | https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library |
| Protocentral MAX30205 Human Body Temperature Sensor Breakout Board |https://github.com/Protocentral/Protocentral_MAX30205|
| Adafruit MPU6050 | https://github.com/adafruit/Adafruit_MPU6050|
|U8g2_Arduino: Arduino Monochrome Graphics Library| https://github.com/olikraus/U8g2_Arduino|

You shall use them as in **examples** folder in each repository.

## Undefined Symbol error

In case of undefined symbol error, you are using some function that was not included but allowed in the library.

**That's mean that you can do it without the function/variable/define etc... you tried to use**

But, to fix the error you shall find first in which library the symbol belong. Then, find the definition (.cpp/.h) in the library and add files in `Libraries/Components_HAL` folder (`Inc` or `Src`). 