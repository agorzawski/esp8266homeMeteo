/**
 * 2017/2018 arek gorzawski
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include "task.hpp"
#include "DataBufferManager.h"
#include "MqttHandler.h"

#define SENSORS_NB 2
#define ONEWIRE_WAIT_AFTER_READOUT_MS 900
#define ONEWIRE_READ_ATTEMPT 3000
#define ONEWIRE_HARDWARE_DISCONNECTED -127.0

using namespace Tasks;
class OneWireDataCollector : public Task
{
 public:
   enum class State
   {
     IDLE,
     REQUESTED,
     READY
   };

      OneWireDataCollector(int bus)
      {
         OneWire _oneWire(bus);
         _sensors = DallasTemperature(&_oneWire);
         _sensors.begin();
      }

      void registerBuffersData(DataBufferManager& data)
      {
          _data = &data;
          for (int i = 0; i < SENSORS_NB; i++)
          {
            //FIXME get those values from file -> editable from WEB
            _bufferId[i] = _data -> getId("degC ","[onewire]");
          }
      }

      void setMqttHandler(MqttHandler& mqttHandler, char* topic){
        _mqttHandler = &mqttHandler;
        _mqttTopic = topic;
      }

      virtual void run()
      {
         long now = millis();
         if (now - _millisOnLastCheck > ONEWIRE_READ_ATTEMPT &&  _state == State::IDLE)
         {
             _sensors.requestTemperatures();
             _millisOnLastCheck = millis();
             delay(ONEWIRE_WAIT_AFTER_READOUT_MS);
             _state = State::REQUESTED;
         }

         if (now - _millisOnLastCheck > ONEWIRE_WAIT_AFTER_READOUT_MS &&  _state == State::REQUESTED)
         {
             _state = State::READY;
         }

        if (_state == State::READY)
        {
          for (int i = 0; i < SENSORS_NB; i++)
          {
               float temp = _sensors.getTempCByIndex(i);
               if (temp == ONEWIRE_HARDWARE_DISCONNECTED)
               {
                 logPrintf("[OneWire] Temperature [%d] is disconnected.", i+1);
                 continue;
               }
               logPrintf("[OneWire] Temperature [%.1f] [stC].", temp);
               if (_data != NULL)
               {
                 _data -> updateData(_bufferId[i], temp);
               }
           }
           _millisOnLastCheck = now;
           _state = State::IDLE;
           logPrintf("[OneWire] Went to IDLE, waiting...");
        }
      }

private:
    DallasTemperature _sensors;
    MqttHandler* _mqttHandler = NULL;
    DataBufferManager* _data = NULL;
    State _state = State::IDLE;

    uint32_t _bufferId[SENSORS_NB];
    long _millisOnLastCheck = 0;
    char* _mqttTopic = "home/test";
    char _msg[75];
};
