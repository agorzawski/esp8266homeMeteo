/**
 * 2017 arek gorzawski
 */

#include "BufferedMeteoData.h"
#include "TimerOnChannel.h"
#include "I2cDataCollector.h"
#include "OneWireDataCollector.h"

#define P0 1013.25

#define PIN_SCL 12  // D6
#define PIN_SDA 13  // D7

#define PIN_LED 2  // D4
#define PIN_ONE_WIRE 0 //  D3

#define PIN_RELAY_1 4  // D2
#define PIN_RELAY_2 5  // D1

#define PIN_SPARE_1 16 // D0
#define PIN_SPARE_2 14 // D5
//#define PIN_SPARE_3 12 // D6

//#define PIN_SCL 15 -> D8 -> not connected in board v1

I2cDataCollector tempPressureCollector(PIN_SDA, PIN_SCL);
OneWireDataCollector tempCollector(PIN_ONE_WIRE);

BufferedMeteoData data;

TimerOnChannel channel1(PIN_RELAY_1, "Fan");
TimerOnChannel channel2(PIN_RELAY_2, "Fan2");

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  tempPressureCollector.registerBuffersData(data);
  tempCollector.registerBuffersData(data);
}

void loop()
{
  blinkStatus(PIN_LED, 50,50);
  blinkStatus(PIN_LED, 50,50);
  
  blinkStatus(PIN_RELAY_1, 250, 50);
  tempPressureCollector.collect();

  blinkStatus(PIN_RELAY_2, 250, 50);
  tempCollector.collect();
  
  blinkStatus(PIN_LED, 50, 50);  
  data.printBuffersStatus();
}


void blinkStatus(int ledId, int high, int low) 
{
  digitalWrite(ledId, HIGH);
  delay(high);
  digitalWrite(ledId, LOW);
  delay(low);
}


