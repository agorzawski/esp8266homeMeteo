/**
 * 2017 arek gorzawski
 */
#include <OneWire.h>
#include <DallasTemperature.h>

class OneWireDataCollector
{
 public:
  OneWireDataCollector(int bus)
  {
     OneWire oneWire(bus);
     _sensors = DallasTemperature(&oneWire);
     _sensors.begin();
  }

  void registerBuffersData(BufferedMeteoData& data)
  {
      _data = &data;
  }
  
  double* collect()
  {
     delay(100);
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
  void setNumberOfSensors(int sensorsNb)
  {
      if (sensorsNb > 0 && sensorsNb < 10)
      {
        _sensorsNb = sensorsNb;
      }  
  }  
private:
  BufferedMeteoData* _data = NULL;
  DallasTemperature _sensors;
  int _sensorsNb = 3;
};
