/**
 * 2017 arek gorzawski
 */
#include "BMP280.h"
#include "MAX44009.h"
#include "Wire.h"
#include "MeteoUtils.h"

#include "task.hpp"
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

    I2cDataCollector(int pin_sda, int pin_scl, DataBufferManager& data)
    {
      I2cDataCollector(pin_sda, pin_scl);
      registerBuffersData(data);
    }

    void registerBuffersData(DataBufferManager& data)
    {
        _data = &data;
        _bufferIdTemp = _data -> getId( "degC","bmp280");
        _bufferIdPressure = _data -> getId( "hPa","bmp280");
        _bufferIdLux = _data -> getId( "lux","max44009");
    }

    virtual void run()
    {
         if ((millis() - _millisOnLastCheck) > 1000)
         {
              double T,P;
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
                            delay(10);
                            _data -> updateData(_bufferIdPressure, getNormalizedPressure(P, 406));
                          }
                     }
              }

              float lux = _light.get_lux();
              if (_data != NULL)
              {
                _data -> updateData(_bufferIdLux, lux);
              }
              _millisOnLastCheck = millis();
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
};
