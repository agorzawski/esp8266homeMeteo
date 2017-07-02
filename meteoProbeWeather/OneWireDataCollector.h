/**
 * 2017 arek gorzawski
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include "task.hpp"
#define SENSORS_NB 2

using namespace Tasks;
class OneWireDataCollector : public Task
{
 public:
      OneWireDataCollector(int bus)
      {
         OneWire oneWire(bus);
         _sensors = DallasTemperature(&oneWire);
         _sensors.begin();
      }

      OneWireDataCollector(int bus, DataBufferManager& data)
      {
         //FIXME if calling the other constructor gets the 'bus shadows a parameter'
         OneWire oneWire(bus);
         _sensors = DallasTemperature(&oneWire);
         _sensors.begin();
        registerBuffersData(data);
      }

      void registerBuffersData(DataBufferManager& data)
      {
          _data = &data;
          for (int i = 0; i < SENSORS_NB; i++)
          {
            _bufferId[i] = _data -> getId("degC [onewire]");
          }
      }

      virtual void run()
      {
         if (millis() - _millisOnLastCheck > 900)
         {
             _sensors.requestTemperatures();
             for (int i = 0; i < SENSORS_NB; i++)
             {
                  double temp = _sensors.getTempCByIndex(i);
                  if (temp <= -127.0)
                  {
                    logPrintf("[OneWire] Temperature [%d] is disconnected.\n", i+1);
                    continue;
                  }
                  if (_data != NULL)
                  {
                    _data -> updateData(_bufferId[i], temp);
                  }
              }
             _millisOnLastCheck = millis();
         }
      }

private:
    DataBufferManager* _data = NULL;
    uint32_t _bufferId[SENSORS_NB];
    long _millisOnLastCheck = 0;
    DallasTemperature _sensors;
};
