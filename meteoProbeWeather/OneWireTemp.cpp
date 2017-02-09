#include "OneWireTemp.h"
#include <OneWire.h>

OneWireTemp::OneWireTemp(int busId)
{
   OneWire oneWire(busId);
   _sensors = DallasTemperature(&oneWire);
   _sensors.begin();
}


void OneWireTemp::setNumberOfSensors(int size)
{
  if (size > 0 && size < 10)
  {
    _sensorsNb = size;
  }
}

double *OneWireTemp::getTemperatures()
{
  delay(300);
  _sensors.requestTemperatures();
  double tempArray[_sensorsNb];
  for (int i=0; i <_sensorsNb; i++)
  {
      double temp = _sensors.getTempCByIndex(i);
      if (temp <= -127.0)
      {
        Serial.printf("[OneWire]Temperature [%d] is disconnected.\n",i+1);
        continue;
      }
      Serial.printf("[OneWire]Temperature [%d] is: ",i+1);
      tempArray[i] = temp;
      Serial.print(temp);
      Serial.print(" degC\n");
  }
 
  return tempArray;
}

