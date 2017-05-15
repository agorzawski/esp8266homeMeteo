#include "OLEDDisplay.h"

/*
REMEMBER that I HARDCODED i2c pins in the Adafruit.cpp file!!, TODO: expose it up!
*/

OLEDDisplay oledDisplay;

void setup()
{
  Serial.begin(115000);
}

void loop()
{
   float temp = 1.0;
  for (int i=0; i < 1000; i++)
  {
    oledDisplay.updateTemperatureDisplay(temp);
    Serial.println(temp, 1);
    delay(2000);   
    temp += 0.2*i;
  }
} 
