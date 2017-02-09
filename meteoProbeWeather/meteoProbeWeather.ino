/**
 * 2017 arek gorzawski
 */

#include "BufferedMeteoData.h"

#include "TimerOnChannel.h"

#include "I2cDataCollector.h"
#include "OneWireDataCollector.h"

#define P0 1013.25

#define PIN_SCL 12
#define PIN_SDA 13

#define PIN_LED 2
#define PIN_ONE_WIRE 0

#define PIN_RELAY_1 4
#define PIN_RELAY_2 5

#define PIN_SPARE_1 14
#define PIN_SPARE_2 15

I2cDataCollector dataCollector(PIN_SDA, PIN_SCL);
OneWireDataCollector temperature(PIN_ONE_WIRE);

BufferedMeteoData data;

TimerOnChannel channel1(PIN_RELAY_1, "Fan");
TimerOnChannel channel2(PIN_RELAY_2, "Fan2");

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  dataCollector.registerBuffersData(data);
  temperature.registerBuffersData(data);

}

void loop()
{
  blinkStatusLED(50,50);
  
  dataCollector.collect();
  blinkStatusLED(50, 50);
  temperature.collect();
  
  blinkStatusLED(100, 200);  
  data.printBuffersStatus();
}


void blinkStatusLED(int high, int low) 
{
  digitalWrite(PIN_LED, HIGH);
  delay(high);
  digitalWrite(PIN_LED, LOW);
  delay(low);
}


