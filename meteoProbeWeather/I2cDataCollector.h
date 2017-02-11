/**
 * 2017 arek gorzawski
 */
#include "BMP280.h"
#include "MAX44009.h"
#include "Wire.h"

class I2cDataCollector
{
  public: 
    I2cDataCollector(int pin_sda, int pin_scl)
    {
        _light.begin(pin_sda, pin_scl);
        _temperaturePressure.begin(pin_sda, pin_scl);
        _temperaturePressure.setOversampling(4);     
    }

    void registerBuffersData(BufferedMeteoData& data)
    {
      _data = &data;
      _bufferIdTemp = _data->getId();
      _bufferIdPressure = _data->getId();
    }

    void collect()
    {
        double T,P;
        char result = _temperaturePressure.startMeasurment();
        if(result != 0){
          delay(result);
          result = _temperaturePressure.getTemperatureAndPressure(T,P);
            if(result != 0)
            {
              if (_data != NULL)
              {
                _data->updateTemp(T);
                _data->updatePressure(P);
              }
              //Serial.print("[bmp280]Altitude = ");Serial.print(_temperaturePressure.altitude(P, P0), 2); Serial.println(" m\n");
            }
        }
        
        float lux = _light.get_lux();
        if (_data != NULL)
        {        
          _data->updateIlluminance(lux);
        }
    }

    private:
      BufferedMeteoData* _data = NULL;
      uint32_t  _bufferIdTemp;
      uint32_t  _bufferIdPressure;
      BMP280 _temperaturePressure;
      MAX44009 _light;
};

