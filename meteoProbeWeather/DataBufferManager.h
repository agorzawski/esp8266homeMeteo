/**
 * 2017 arek gorzawski
 */
#ifndef DATABUFFERMANAGER_h
#define DATABUFFERMANAGER_h

#include "task.hpp"
#include "BufferedMeteoData.h"
#include "MqttHandler.h"
#include <math.h>

#define TIME_TO_AGGREGATE 5000
#define TENDENCE_SIZE 3

using namespace Tasks;
class DataBufferManager : public Task
{
 public:

     enum class BufferLength
     {
       MINUTE,
       HOUR,
       SIXTY_HOURS
     };

      DataBufferManager(BufferedMeteoData& data, BufferedMeteoData& data1h, BufferedMeteoData& data60h)
      {
        _data = &data;
        _data1h = &data1h;
        _data60h = &data60h;

        for (int i = 0; i < TENDENCE_SIZE; i++)
        {
          _tendence[i] = 0.0;
        }
      }

      void setMqttHandler(MqttHandler& mqttHandler){
        _mqttHandler = &mqttHandler;
      }

      uint32_t  getId(char* label, char* sensor)
      {
        uint32_t  id = _data -> getId(label, sensor);
        uint32_t  id2 = _data1h -> getId(label, sensor);
        uint32_t  id3 = _data60h -> getId(label, sensor);
        return id;
      }

      size_t getUsed(uint32_t id, BufferLength length)
      {
          if (BufferLength::MINUTE == length) return _data -> getUsed(id);
          if (BufferLength::HOUR == length) return _data1h -> getUsed(id);
          if (BufferLength::SIXTY_HOURS == length) return _data60h -> getUsed(id);
      }

      void updateData(uint32_t id, float value)
      {
        _data -> updateData(id, value);
      }

      float getCurrentData(uint32_t id)
      {
          return _data -> getData(id);
      }

      float* getDataAll(uint32_t id, BufferLength length)
      {
        //TODO dodac sensowny zwrot: jedna tablica z TS i jedna z Vartosciami
        if (BufferLength::MINUTE == length) return _data -> getDataAll(id);
        if (BufferLength::HOUR == length) return _data1h -> getDataAll(id);
        if (BufferLength::SIXTY_HOURS == length) return _data60h -> getDataAll(id);
      }

      float getLastAggregatedData(uint32_t id)
      {
          return _data1h -> getData(id);
      }

      float getTendence(uint32_t id)
      {
          return _tendence[id];
      }

      virtual void run()
      {
        sleep(60_s);
        logPrintf(" 1mins  agregation start...\n");
        for (int id = 0; id < 2; id++)
        {
          aggregateLast(id, TIME_TO_AGGREGATE);
          updateTendences(id);
        }
        logPrintf(" 1mins agregation DONE.\n");

      }

      String getActualTendenceLabel()
      {
        return _label;
      }

    private:

      BufferedMeteoData* _data = NULL;
      BufferedMeteoData* _data1h = NULL;
      BufferedMeteoData* _data60h = NULL;
      MqttHandler* _mqttHandler = NULL;

      String _label = "";
      float _tendence[TENDENCE_SIZE];

      void aggregateLast(uint32_t id, uint32_t time)
      {
          uint16_t toCollect = 60;

  //        if (_data -> getUsed(id) == _data -> getSize(id))
  //        {
            float* lastSamples = _data -> getDataAll(id, toCollect);
            float avg1min = avgData(lastSamples, toCollect);

            if (!std::isinf(avg1min))
            {
              _data1h -> updateData(id, avg1min);
            }
//          }

//          if (_data1h -> getUsed(id) == _data1h -> getSize(id))
//          {
            float* lastSamples1h = _data1h -> getDataAll(id, toCollect);
            float avg1h = avgData(lastSamples1h, toCollect);

            if (!std::isinf(avg1h))
            {
              _data60h -> updateData(id, avg1h);
            }
  //        }
      }

      void updateTendences(uint32_t id)
      {
        float* twoPoints = _data1h -> getDataAll(id, 2);
        _label = "/min";
        _tendence[id] = twoPoints[1] - twoPoints[0];

        Serial.print(twoPoints[0], 2); Serial.printf("\n");
        Serial.print(twoPoints[1], 2); Serial.printf("\n");
        Serial.print(_tendence[id], 2); Serial.printf("\n");
      }

      float avgData(float* data, uint16_t size)
      {
        float sum = 0;
        for (int i = 0; i < size; i++){
          sum += data[i];
        }
        return sum / size;
      }

};
#endif
