/**
 * 2017 arek gorzawski
 */
#include "BMP280.h"
#include "MAX44009.h"
#include "Wire.h"
#include "MeteoUtils.h"
#include "MqttHandler.h"
#include "task.hpp"

#define DELAY_BETWEEN_REDOUTS_MS 20
#define DEFAULT_ALT_M 406
#define DEFUALT_MQTT_PUBLISH_TIME 30000
#define DEFUALT_PUBLISH_TIME 1000
#define MAX_MQTT_MESSAGE_LEMGTH 75

using namespace Tasks;

class I2cDataCollector : public Task
{
  public:
    I2cDataCollector(int pin_sda, int pin_scl)
    {
        _light.begin(pin_sda, pin_scl);
        _temperaturePressure.begin(pin_sda, pin_scl);
        _temperaturePressure.setOversampling(4);
    }

    void setMqttHandler(MqttHandler& mqttHandler, String topic){
      _mqttHandler = &mqttHandler;
      _mqttTopic = topic;
      logPrintf("[i2c coll] Registred for meteo params on (%s)", _mqttTopic.c_str());
    }

    void registerBuffersData(DataBufferManager& data)
    {
        _data = &data;
        //FIXME get those values from file -> editable from WEB
        _bufferIdTemp = _data -> getId( "degC","temp");
        _bufferIdPressure = _data -> getId( "hPa","pressure");
        _bufferIdLux = _data -> getId( "lux","lumi");
    }

    virtual void run()
    {
         if ((millis() - _millisOnLastCheck) > DEFUALT_PUBLISH_TIME)
         {
              double T,P,tempPressure;
              char result = _temperaturePressure.startMeasurment();
              if(result != 0)
              {
                  delay(result);
                  result = _temperaturePressure.getTemperatureAndPressure(T,P);
                      if(result != 0)
                      {
                          if (_data != NULL)
                          {
                            _data -> updateData(_bufferIdTemp, T);
                            delay(DELAY_BETWEEN_REDOUTS_MS);
                            tempPressure =  getNormalizedPressure(P, DEFAULT_ALT_M);
                            _data -> updateData(_bufferIdPressure, tempPressure);
                          }

                          if ((millis() - _millisOnLastPublish) > DEFUALT_MQTT_PUBLISH_TIME)
                          {
                            snprintf (_msg, MAX_MQTT_MESSAGE_LEMGTH, "{\"t\":%.1f, \"p\": %.1f}", T, tempPressure);
                            if (_mqttHandler != NULL)
                            {
                              _mqttHandler -> publish(_mqttTopic.c_str(), _msg);
                            }
                            _millisOnLastPublish = millis();
                          }
                     }
              }
              delay(DELAY_BETWEEN_REDOUTS_MS);
              float lux = _light.get_lux();
              if (_data != NULL)
              {
                //FIXME sensor?
                _data -> updateData(_bufferIdLux, lux);
                //Serial.print("lux : ");Serial.print(lux, 2); Serial.printf("\n");
              }
              _millisOnLastCheck = millis();
              logPrintf("[I2C] Went to IDLE, waiting...");
         }
    }

    private:
        DataBufferManager* _data = NULL;
        uint32_t _bufferIdTemp;
        uint32_t _bufferIdPressure;
        uint32_t _bufferIdLux;
        BMP280 _temperaturePressure;
        MAX44009 _light;
        long _millisOnLastCheck = 0;
        long _millisOnLastPublish = 0;
        MqttHandler* _mqttHandler = NULL;
        float defualtAltitudeForReference = DEFAULT_ALT_M;

        String _mqttTopic;
        char _msg[MAX_MQTT_MESSAGE_LEMGTH];
};
