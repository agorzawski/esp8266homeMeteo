/**
 * 2017 arek gorzawski
 */
#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSORS_NB 2

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
          for (int i = 0; i < SENSORS_NB; i++)
          {
            _bufferId[i] = _data -> getId("degC [onewire ]");         
          }         
      }
      
      void collect()
      {
         _sensors.requestTemperatures();    
         for (int i = 0; i < SENSORS_NB; i++)
         {
              double temp = _sensors.getTempCByIndex(i);
              if (temp <= -127.0)
              {
                Serial.printf("[OneWire] Temperature [%d] is disconnected.\n",i+1);
                continue;
              }
              Serial.printf("[OneWire]Temperature [%d] is: ",i+1);
              Serial.print(temp);
              Serial.print(" degC\n");
              //_data -> updateData(_bufferId[i], temp);
          }     
      }
  
private:
    BufferedMeteoData* _data = NULL;
    uint32_t _bufferId[SENSORS_NB]; 
    DallasTemperature _sensors; 
};
