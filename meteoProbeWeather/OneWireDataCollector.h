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

#define DEFUALT_MQTT_PUBLISH_TIME 30000
#define DEFUALT_PUBLISH_TIME 1000

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

      OneWireDataCollector(int bus):
        _oneWire(bus),
        _sensors(&_oneWire)
      {        
         //_sensors = DallasTemperature(&_oneWire);
         _sensors.begin();
         _sensors.setWaitForConversion(false);
         _sensors.requestTemperatures();
         sleep(10_s);
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

      void setMqttHandler(MqttHandler& mqttHandler, String topic){
        _mqttHandler = &mqttHandler;
        _mqttTopic = topic;
        logPrintf("[OneWire] Registred for meteo params on (%s)", _mqttTopic.c_str());
      }

      virtual void run()
      {
        long now = millis();
        _state = State::READY;
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
               if (_data != NULL && (millis() - _millisOnLastPublish) > DEFUALT_MQTT_PUBLISH_TIME)
               {
                 snprintf (_msg, 75, "{\"t\":%.2f}", temp);
                 if (_mqttHandler != NULL)
                 {
                   _mqttHandler -> publish(_mqttTopic.c_str(), _msg);
                 }
                 _millisOnLastPublish = millis();
               }
           }
           _millisOnLastCheck = now;
           _state = State::IDLE;
           _sensors.requestTemperatures();
           logPrintf("[OneWire] Went to IDLE, waiting...");
           sleep(10_s);
      }

private:
    OneWire _oneWire;
    DallasTemperature _sensors;
    MqttHandler* _mqttHandler = NULL;
    DataBufferManager* _data = NULL;
    State _state = State::READY;
    uint32_t _bufferId[SENSORS_NB];
    long _millisOnLastCheck = 0;
    long _millisOnLastPublish = 0;
    String _mqttTopic = "home/test";
    char _msg[75];
};
