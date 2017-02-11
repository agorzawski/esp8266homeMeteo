/**
 * 2017 arek gorzawski
 */

#include "CircularBuffer.h"
#include "utils.h"
#include <vector>

#define BUFFER_SIZE 1000

class BufferedMeteoData
{
  public:

    uint32_t getId();
    void update(float value, uint32_t id);
    float* get(uint32_t id);

    
    void updateTemp(float newValue);
    void updatePressure(float newValue);
    void updateIlluminance(float newValue);
    float* getPressure();
    float* getTemperature();
    float* getIlluminance();
    
    void printBuffersStatus();
    
  private:
  
    CircularBuffer<float, BUFFER_SIZE> _temperature;
    CircularBuffer<float, BUFFER_SIZE> _pressure;
    CircularBuffer<float, BUFFER_SIZE> _illuminance;
    CircularBuffer<long, BUFFER_SIZE> _time;

    std::vector<CircularBuffer<float, BUFFER_SIZE>> _data;
      
    uint32_t _id = 1;
    
};

