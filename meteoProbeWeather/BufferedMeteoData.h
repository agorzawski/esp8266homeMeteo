#include "CircularBuffer.h"

#define BUFFER_SIZE 1000

class BufferedMeteoData
{
  public:
    void updateTemp(float newValue);
    void updatePressure(float newValue);
    void updateIlluminance(float newValue);

    float* getPressure();
    float* getTemperature();
    float* getIlluminance();

    void printBuffersStatus();
    
  private:
    CircularBuffer<BUFFER_SIZE> _temperature;
    CircularBuffer<BUFFER_SIZE> _pressure;
    CircularBuffer<BUFFER_SIZE> _illuminance;
};

