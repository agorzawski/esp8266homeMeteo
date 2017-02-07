#include "BMP280.h"
#include "MAX44009.h"
#include "OneWireTemp.h"
#include "Wire.h"

#define P0 1013.25

#define PIN_SCL 12
#define PIN_SDA 13

#define PIN_LED 2
#define PIN_ONE_WIRE 0

#define PIN_RELAY_1 4
#define PIN_RELAY_2 5

#define PIN_SPARE_1 14
#define PIN_SPARE_2 15

#define BUFFER_SIZE 100


BMP280 temperaturePresure;
MAX44009 light;
OneWireTemp temperature(PIN_ONE_WIRE);

double temperatureBuffer[BUFFER_SIZE];
double pressureBuffer[BUFFER_SIZE];
double illuminence[BUFFER_SIZE];

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  
  light.begin(PIN_SDA, PIN_SCL);
  
  temperaturePresure.begin(PIN_SDA, PIN_SCL);
  temperaturePresure.setOversampling(4);
}

void loop()
{
  blinkStatusLED(50,50);
  
  double T,P;
  char result = temperaturePresure.startMeasurment();
  if(result!=0){
    delay(result);
    result = temperaturePresure.getTemperatureAndPressure(T,P);
      if(result!=0)
      {
        Serial.print("[bmp280]Temperature = ");Serial.print(T,2); Serial.print(" degC\t\n");
        Serial.print("[bmp280]Pressure = ");Serial.print(P,2); Serial.print(" mBar\t\n");
        Serial.print("[bmp280]Altitude = ");Serial.print(temperaturePresure.altitude(P,P0),2); Serial.println(" m\n");
      }
  }
  
  blinkStatusLED(50,50);
  float lux = light.get_lux();
  Serial.print("[max440]Illuminance = ");Serial.print(lux,2); Serial.print(" lux\n");

  blinkStatusLED(100, 100);
  
  temperature.getTemperatures();
  blinkStatusLED(100, 100);  
}


void blinkStatusLED(int high, int low) 
{
  digitalWrite(PIN_LED, HIGH);
  delay(high);
  digitalWrite(PIN_LED, LOW);
  delay(low);
}


