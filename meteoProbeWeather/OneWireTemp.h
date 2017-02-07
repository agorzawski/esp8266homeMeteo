#include <DallasTemperature.h>

class OneWireTemp
{
 public:
  
  OneWireTemp(int bus);
  double* getTemperatures();
  void setNumberOfSensors(int sensorsNb);
  
private:
  DallasTemperature _sensors;
  int _sensorsNb = 3;
};

