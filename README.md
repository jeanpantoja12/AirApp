# AirApp

An Arduino project based on Oximeter medition and control with Bluetooth conection through Arduino UNO board to check patients with respiratory diseases.

## Principal Components
- Arduino UNO 
- HC-05 Bluetooth
- MAX30102 Sensor

## How to Run

You will need to install some libraries to run this project

```c++
#include "MAX30105.h"
#include <SoftwareSerial.h>
#include "spo2_algorithm.h"
```
Then, just copy and run the project on your board. Check Arduino pins and distribution to avoid malfunctions.
