/**
 * 2017 arek gorzawski
 */
 
#include "BufferedMeteoData.h"
#include <Arduino.h>

void BufferedMeteoData::printBuffersStatus()
{
  //Serial.printf("[buffers] Pressure %d / %d , Temperature %d / %d, Illuminance %d / %d \n",_pressure.getUsed(), BUFFER_SIZE, _temperature.getUsed(), BUFFER_SIZE, _illuminance.getUsed(), BUFFER_SIZE);
  logPrintf("[buffers] Pressure %d / %d , Temperature %d / %d, Illuminance %d / %d \n", _pressure.getUsed(), BUFFER_SIZE, _temperature.getUsed(), BUFFER_SIZE, _illuminance.getUsed(), BUFFER_SIZE);

}

void BufferedMeteoData::updateTemp(float newTemp)
{
  Serial.print("[bmp280]Temperature = ");Serial.print(newTemp,2); Serial.print(" degC\t\n");
  //logPrintf("[bmp280]Temperature = %4.2f  degC", newTemp);
  
  _temperature.write(&newTemp, 1);
}

void BufferedMeteoData::updatePressure(float newPressure)
{
  Serial.print("[bmp280]Pressure = ");Serial.print(newPressure,2); Serial.print(" mBar\t\n");
  //logPrintf("[bmp280]Pressure = %4.2f  mBar", newPressure);
  
  _pressure.write(&newPressure, 1);
}

void BufferedMeteoData::updateIlluminance(float newValue)
{
  Serial.print("[max440]Illuminance = ");Serial.print(newValue,2); Serial.print(" lux\n"); 
  //logPrintf("[max440]Illuminance = %4.2f  lux", newValue);
  
  _illuminance.write(&newValue, 1);
}

float* BufferedMeteoData::getIlluminance()
{
   float* toReturn;
  _illuminance.read(toReturn, _illuminance.getUsed());
  _illuminance.reset();
  return toReturn;
}

float* BufferedMeteoData::getPressure()
{
  float* toReturn;
  _pressure.read(toReturn, _pressure.getUsed());
  _pressure.reset();
  return toReturn;
}

float* BufferedMeteoData::getTemperature()
{
  float* toReturn;
  _temperature.read(toReturn, _temperature.getUsed());
  _temperature.reset();
  return toReturn;
}


