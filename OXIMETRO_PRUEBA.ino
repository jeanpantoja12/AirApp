#include <TimerOne.h>
#include <Wire.h>
#include "MAX30105.h"
#include <SoftwareSerial.h>
#include "spo2_algorithm.h"
// declaracion de variables
bool is_sending = false;
byte valSPO2;
int cont = 0;
int i;
int RecData;
volatile int flgTx = 0;
//Definicion MAX30102------------------------------------------------------
MAX30105 particleSensor;
#define MAX_BRIGHTNESS 255
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit form
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100]; //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100]; //red LED sensor data
#endif
int32_t bufferLength = 100; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
//byte pulseLED = 11; //Must be on PWM pin
//byte readLED = 13; //Blinks with each data read
//Fin definicion MAX30102--------------------------------------------------
SoftwareSerial BT1(10,11);

void setup() {
  BT1.begin(9600);
  Serial.begin(115200); //Serial que usa el modulo Bluetooth
  // configuracion sensor SPO2----------------------------------------------------------------------------
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 2; //Options: 1, 2, 4, 8, 16, 32 // En 1
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200 // En 1000
  int pulseWidth = 411; //Options: 69, 118, 215, 411 / En 69
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384 / En 2048
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  //------------------------------------------------------------------------------------------------------
}
// funcion toma de datos principal


void loop() {
  if (BT1.available()>0) {
    Serial.println("En BT");
    // Comprobamos si ha llegado algún dato
    RecData = BT1.read(); // Se lee el dato recibido
    Serial.print("BT send: "+ RecData);
    if (RecData == 49) { // Si el dato recibido es 170 se realiza la medida
      is_sending = true;
    }
    else {
      //Timer1.stop();
    }
  }
  if(is_sending){
    for (byte i = 0 ; i < bufferLength ; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data
      
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
      
    }
    //calculate heart rate and SpO2
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    //After gathering 25 new samples recalculate HR and SP02
    //Final toma de datos de SPO2---------------------------------------------------------------------
    if (validSPO2 == 1) {
      valSPO2 = spo2;
    } else {
      valSPO2 = 0;
    }
  }
  if(is_sending){
    Serial.println(valSPO2);
    BT1.print(valSPO2);
    BT1.print(";");
    is_sending = false;
  }
  delay(20);
//envio de datos a traves del puerto serie bluetooth
}
