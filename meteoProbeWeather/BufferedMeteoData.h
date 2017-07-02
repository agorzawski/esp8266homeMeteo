/**
 * 2017 arek gorzawski
 */
#pragma once
#include "CircularBuffer.h"
#include "utils.h"
#include <vector>

#define BUFFER_SIZE 60
#define VERBOSE 1

class BufferedMeteoData
{
  public:

    uint32_t getId(char* label)
    {
        CircularBuffer<float, BUFFER_SIZE> dataBuffer;
        CircularBuffer<uint32_t, BUFFER_SIZE> timeBuffer;

        _times.push_back(timeBuffer);
        _data.push_back(dataBuffer);
        _units.push_back(label);

        logPrintf("Assigned buffer nb. %d to values [%s]\n", _id, label);
        return _id++;
    }

    void updateData(uint32_t id, float value)
    {
        //uint32_t t = 7;
        _data[id].write(&value, 1);
        //_times[id].write(0, 1);
        //Serial.print(value, 2); Serial.printf("[%s]\t\n", _units[id]);
    }

    float getData(uint32_t id)
    {
        return _data[id].read();
    }

    float* getDataAll(uint32_t id)
    {
      float* toReturn = new float[BUFFER_SIZE];
       _data[id].read(toReturn, _data[id].getSize());
      return toReturn;
    }

    float* getDataAll(uint32_t id, uint32_t size)
    {
      float* toReturn = new float[size];
       _data[id].read(toReturn, size);
      return toReturn;
    }

    void printBuffersStatus()
    {
        if (VERBOSE)
        {
          for (uint32_t id = 0; id < _id; id++)
          {
            logPrintf("[time] %d /%d [%s] %d / %d \n", _times[id].getUsed(), BUFFER_SIZE, _units[id], _data[id].getUsed(), BUFFER_SIZE);
          }
        }
    }

  private:
      std::vector<CircularBuffer<float, BUFFER_SIZE>> _data;
      std::vector<CircularBuffer<uint32_t, BUFFER_SIZE>> _times;
      std::vector<char*> _units;
      uint32_t _id = 0;
};
