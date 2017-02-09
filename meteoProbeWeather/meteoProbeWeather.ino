#include "BMP280.h"
#include "MAX44009.h"
#include "OneWireTemp.h"
#include "Wire.h"
#include "BufferedMeteoData.h"
#include "TimerOnChannel.h"

#define P0 1013.25

#define PIN_SCL 12
#define PIN_SDA 13

#define PIN_LED 2
#define PIN_ONE_WIRE 0

#define PIN_RELAY_1 4
#define PIN_RELAY_2 5

#define PIN_SPARE_1 14
#define PIN_SPARE_2 15

BMP280 temperaturePressure;
MAX44009 light;
OneWireTemp temperature(PIN_ONE_WIRE);
BufferedMeteoData data;

TimerOnChannel channel1(PIN_RELAY_1, "Fan");
TimerOnChannel channel2(PIN_RELAY_2, "Fan2");


void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  light.begin(PIN_SDA, PIN_SCL);
  temperaturePressure.begin(PIN_SDA, PIN_SCL);
  temperaturePressure.setOversampling(4);
}

void loop()
{
  blinkStatusLED(50,50);
  
  double T,P;
  char result = temperaturePressure.startMeasurment();
  if(result!=0){
    delay(result);
    result = temperaturePressure.getTemperatureAndPressure(T,P);
      if(result!=0)
      {
        data.updateTemp(T);
        data.updatePressure(P);
        Serial.print("[bmp280]Altitude = ");Serial.print(temperaturePressure.altitude(P, P0), 2); Serial.println(" m\n");
      }
  }
  
  blinkStatusLED(50,50);  
  float lux = light.get_lux();
  data.updateIlluminance(lux);
  
  blinkStatusLED(50, 50);
  temperature.getTemperatures();
  
  blinkStatusLED(100, 200);  
  data.printBuffersStatus();
}


void blinkStatusLED(int high, int low) 
{
  digitalWrite(PIN_LED, HIGH);
  delay(high);
  digitalWrite(PIN_LED, LOW);
  delay(low);
}


